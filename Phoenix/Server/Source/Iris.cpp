// Copyright 2019-20 Genten Studios
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <Server/Iris.hpp>

#include <Common/GameNetwork.hpp>
#include <Common/Logger.hpp>
#include <Common/Network/Network.hpp>
#include <Common/Serialization/Serializer.hpp>
#include <Common/Settings.hpp>

#include <Common/Actor.hpp>
#include <Common/Movement.hpp>
#include <Common/Position.hpp>

using namespace phx::server;

// simple number, leaves us headroom, also idk the enet max.
static constexpr std::size_t MAX_USERS = 1024;

Iris::Iris(entt::registry* registry) : m_registry(registry)
{
	// get required settings.
	m_maxClients =
	    phx::Settings::get()->add("Max Users", "core:net_max_players", 32);
	m_maxClients->setMax(MAX_USERS);
	m_maxClients->setMin(1);

	// we can make these configurable down the line.
	m_serverName    = "Phoenix Server";
	m_serverMessage = "dis is da phoenix server.";

	// initialize network.
	phx::net::Network::initialize();

	// we make these variables since otherwise the Host constructor is not nice
	// to read. we add 5 so we can have "capacity" for people to "connect" and
	// request server info, maybe get rejected, etc... we also don't need to
	// cast to std::size_t but clang-tidy is a bitch and won't leave me alone :(
	const std::size_t maxConnect =
	    static_cast<std::size_t>(m_maxClients->value()) + 5;
	const std::size_t channelCount =
	    static_cast<std::size_t>(phx::NetworkChannels::COUNT);

	m_server = new net::Host(phx::net::Address(7777), maxConnect, channelCount);

	m_server->onConnect(
	    [=](net::Peer& peer, enet_uint32 reason) { onConnect(peer, reason); });

	// m_server.onConnect([this](net::Peer& peer, enet_uint32) {
	//	LOG_INFO("NETWORK")
	//	    << "Client connected from: " << peer.getAddress().getIP();
	//	{
	//		auto entity = m_registry->create();
	//		m_registry->emplace<Player>(
	//		    entity, ActorSystem::registerActor(m_registry), peer.getID());
	//		m_users.emplace(peer.getID(), entity);
	//		eventQueue.push({entity, Event::Type::CONNECT});
	//	}
	//});

	m_server->onReceive(
	    [this](net::Peer& peer, net::Packet&& packet, enet_uint32 channelID) {
		    switch (channelID)
		    {
		    case static_cast<int>(phx::NetworkChannels::GENERAL):
			    break;
		    case static_cast<int>(phx::NetworkChannels::AUTH):
			    authenticateClient(peer, std::forward<net::Packet>(packet));
			    break;
		    case static_cast<int>(phx::NetworkChannels::STATE):
			    break;
		    case static_cast<int>(phx::NetworkChannels::VOXEL_DATA):
			    break;
		    case static_cast<int>(phx::NetworkChannels::MESSAGE):
			    break;
		    default:
			    break;
		    }
	    });

	m_server->onDisconnect([this](std::size_t peerID, enet_uint32 data) {
		onDisconnect(peerID, data);
	});
}

Iris::~Iris() { phx::net::Network::teardown(); }

void Iris::run()
{
	m_running = true;
	while (m_running)
	{
		m_server->poll();
	}
}

void Iris::kill() { m_running = false; }

void Iris::onConnect(net::Peer& peer, enet_uint32 reason)
{
	phx::Serializer ser;

	if (static_cast<NetworkEvents>(reason) == NetworkEvents::GET_SERVER_INFO)
	{
		ser << static_cast<unsigned short>(NetworkEvents::GET_SERVER_INFO);
		// serialize server name.
		ser << m_serverName;
		// use shorts, minimize data size - we will never reach the limit of an
		// integer, or even a short (~65k) but less bytes is better.
		// take away 1 since this client will be immediately disconnected.
		ser << static_cast<unsigned short>(m_server->getPeerCount() - 1)
		    << static_cast<unsigned short>(m_maxClients->value());
		// serialize server message
		ser << m_serverMessage;

		net::Packet packet(ser.getBuffer(), net::PacketFlags::RELIABLE);
		peer.send(packet, static_cast<enet_uint8>(NetworkChannels::GENERAL));
		peer.disconnectOncePacketsAreSent();

		m_server->flush();
	}
	else if (static_cast<NetworkEvents>(reason) == NetworkEvents::CONNECT)
	{
		// we don't create entities or anything yet, we just add them to a list
		// that we want authed.
		m_connectingPlayers.push_back(peer.getID());
	}
}

void Iris::onDisconnect(std::size_t peer, enet_uint32 reason)
{
	const auto it = m_users.find(peer);
	if (it == m_users.end())
	{
		// check if user is in the connecting players list.
		auto connecting = std::find(m_connectingPlayers.begin(),
		                            m_connectingPlayers.end(), peer);
		if (connecting == m_connectingPlayers.end())
		{
			// they have been disconnected already, or were never connected...
			// ?!?
			LOG_DEBUG("NETWORK") << "A peer that has already been disconnected "
			                        "is being disconnected again??";
		}
		else
		{
			m_connectingPlayers.erase(connecting);
		}

		return;
	}

	m_registry->destroy(it->second.entity);

	m_users.erase(it);
}

void Iris::authenticateClient(net::Peer& peer, net::Packet&& packet)
{
	auto it = std::find(m_connectingPlayers.begin(), m_connectingPlayers.end(),
	                    peer.getID());
	if (it == m_connectingPlayers.end())
	{
		if (m_users.find(peer.getID()) != m_users.end())
		{
			// they must no longer be connecting and already authenticated -
			// this would be a programming error so lets print a debug message.
			LOG_DEBUG("NETWORK")
			    << "A player that has already authenticated is "
			       "authenticating again.";
		}

		// this part is impossible afaik, we don't worry about this part of the
		// statement..

		return;
	}	

	Serializer ser;
	ser.setBuffer(packet.getData());

	std::string username;

	ser >> username;

	bool exists = false;
	for (auto& element : m_users)
	{
		if (element.second.name == username)
		{
			exists = true;
			break;
		}
	}

	if (exists)
	{
		Serializer existsSer;
		existsSer << static_cast<unsigned short>(NetworkEvents::INVALID_AUTH);
		existsSer << "A player with the same username is already connected.";

		// send this reliably, why not.
		net::Packet packetToSend(existsSer.getBuffer(), net::PacketFlags::RELIABLE);
		peer.send(packetToSend, static_cast<enet_uint8>(NetworkChannels::AUTH));
		peer.disconnectOncePacketsAreSent();

		return;
	}

	// they can play, we tell them auth is good and dip.
	Serializer existsSer;
	existsSer << static_cast<unsigned short>(NetworkEvents::SUCCESSFUL_AUTH);

	net::Packet packetToSend(existsSer.getBuffer(), net::PacketFlags::RELIABLE);
	peer.send(packetToSend, static_cast<enet_uint8>(NetworkChannels::AUTH));

	std::cout << username << " has connected" << std::endl;
}

// void Iris::disconnect(std::size_t peerID)
//{
//	LOG_INFO("NETWORK") << peerID << " disconnected";
//	m_registry->destroy(m_users.at(peerID));
//}
//
// void Iris::parseEvent(std::size_t userID, Packet& packet)
//{
//	std::string data;
//
//	phx::Serializer ser;
//	ser.setBuffer(packet.getData());
//	ser >> data;
//
//	printf("Event received");
//	printf("An Event packet containing %s was received from %lu\n",
//	       data.c_str(), userID);
//}
//
// void Iris::parseState(std::size_t userID, phx::net::Packet& packet)
//{
//	InputState input;
//
//	auto data = packet.getData();
//
//	phx::Serializer ser;
//	ser.setBuffer(data.data(), packet.getSize());
//	ser >> input;
//
//	// If the queue is empty we need to add a new bundle
//	if (currentBundles.empty())
//	{
//		StateBundle bundle;
//		bundle.sequence = input.sequence;
//		bundle.ready    = false;
//		bundle.users    = 1; ///@todo We need to capture how many users we are
//		/// expecting packets from
//		currentBundles.push_back(bundle);
//	}
//
//	// Discard state if its older that the oldest stateBundle
//	if (input.sequence < currentBundles.front().sequence &&
//	    currentBundles.back().sequence - input.sequence < 10)
//	{
//		printf("discard %lu \n", input.sequence);
//		return;
//	}
//
//	// Fill the stateBundles up to the current input sequence
//	while ((input.sequence > currentBundles.back().sequence &&
//	        input.sequence - currentBundles.back().sequence > 10) ||
//	       currentBundles.back().sequence == 255)
//	{
//		// Insert a new bundle if this is the first packet in this sequence
//		StateBundle bundle;
//		bundle.sequence = currentBundles.back().sequence + 1;
//		bundle.ready    = false;
//		bundle.users    = 1; ///@todo We need to capture how many users we are
//		/// expecting packets from
//		currentBundles.push_back(bundle);
//	}
//
//	{
//		for (auto it = currentBundles.begin(); it != currentBundles.end(); ++it)
//		{
//			StateBundle& bundle = *it;
//			if (bundle.sequence == input.sequence)
//			{
//				// Thread safety! If we said a bundle is ready, were too late
//				if (!bundle.ready)
//				{
//					bundle.states[m_users[userID]] = input;
//					// If we have all the states we need, then the bundle is
//					// ready
//					if (bundle.states.size() >= bundle.users)
//					{
//						bundle.ready = true;
//						stateQueue.push(bundle);
//						currentBundles.erase(it);
//					}
//				}
//				break;
//			}
//		}
//	}
//
//	// If we have more than 10 states enqueued, assume we lost a packet
//	if (currentBundles.size() > 10)
//	{
//		currentBundles.front().ready = true;
//		stateQueue.push(currentBundles.front());
//		currentBundles.erase(currentBundles.begin());
//	}
//}
//
// void Iris::parseMessage(std::size_t userID, phx::net::Packet& packet)
//{
//	std::string input;
//
//	auto data = packet.getData();
//
//	phx::Serializer ser;
//	ser.setBuffer(reinterpret_cast<std::byte*>(data.data()), data.size());
//	ser >> input;
//
//	/// @TODO replace userID with userName
//	std::cout << userID << ": " << input << "\n";
//
//	if (input[0] == '/')
//	{
//		MessageBundle message;
//		message.message = input.substr(1);
//		message.userID  = userID;
//		messageQueue.push(message);
//	}
//	else
//	{
//		sendMessage(userID, input);
//	}
//}
//
// void Iris::sendEvent(std::size_t userID, enet_uint8* data) {}
//
// void Iris::sendState(entt::registry* registry, std::size_t sequence)
//{
//	auto       view = registry->view<Position, Movement>();
//	Serializer ser;
//	ser << sequence;
//	for (auto entity : view)
//	{
//		auto pos = view.get<Position>(entity);
//		ser << pos.position.x << pos.position.y << pos.position.z;
//	}
//	Packet packet = Packet(ser.getBuffer(), PacketFlags::UNRELIABLE);
//	m_server->broadcast(packet, 1);
//}
//
// void Iris::sendMessage(std::size_t userID, const std::string& message)
//{
//	Serializer ser;
//	ser << message;
//	Packet packet = Packet(ser.getBuffer(), PacketFlags::RELIABLE);
//	Peer*  peer   = m_server->getPeer(userID);
//	peer->send(packet, 2);
//}
//
// void Iris::sendData(std::size_t userID, voxels::Chunk* data)
//{
//	Serializer ser;
//	ser << *data;
//	Packet packet = Packet(ser.getBuffer(), PacketFlags::RELIABLE);
//	Peer*  peer   = m_server->getPeer(userID);
//	peer->send(packet, 3);
//}
