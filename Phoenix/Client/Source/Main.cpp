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

#include <Client/Client.hpp>

#include <Common/GameNetwork.hpp>
#include <Common/Network/Network.hpp>
#include <Common/Serialization/Serializer.hpp>

using namespace phx;

#undef main
int main(int argc, char** argv)
{
	// client::Client::get()->run();

	net::Network::initialize();

	net::Host host;

	bool canConnect = false;
	bool full      = false;
	bool connected = false;
	host.onConnect([&connected](net::Peer&, enet_uint32) {
		std::cout << "\nConnected" << std::endl;
		connected = true;
	});
	host.onReceive(
	    [&full, &canConnect](net::Peer& peer, net::Packet&& packet, enet_uint32 channel) {
		    switch (static_cast<NetworkChannels>(channel))
		    {
		    case NetworkChannels::GENERAL:
		    {
			    Serializer ser;
			    ser.setBuffer(packet.getData());

			    unsigned short eventCode;
			    ser >> eventCode;

			    if (static_cast<NetworkEvents>(eventCode) ==
			        NetworkEvents::GET_SERVER_INFO)
			    {
				    std::string    serverName;
				    unsigned short connectedPeers;
				    unsigned short totalPeers;
				    std::string    serverMessage;
		    	
				    ser >> serverName;
				    ser >> connectedPeers;
				    ser >> totalPeers;
				    ser >> serverMessage;

					if (connectedPeers == totalPeers)
					    full = true;
						
				    std::cout << "\n" << serverName << std::endl;
				    std::cout << serverMessage << std::endl;
				    std::cout << std::to_string(connectedPeers) << " / "
				              << std::to_string(totalPeers);

			    	// we've recieved server info, lets keep this in mind so we
				    // can connect afterwards.
				    canConnect = true;
			    }
				else
		    	{
				    std::cout << "Received invalid packet" << std::endl;
		    	}
		    }
		    break;
		    case NetworkChannels::AUTH:
		    {
			    Serializer ser;
			    ser.setBuffer(packet.getData());

		    	unsigned short eventCode;
			    ser >> eventCode;

		    	if (static_cast<NetworkEvents>(eventCode) == NetworkEvents::INVALID_AUTH)
		    	{
				    std::string message;
				    ser >> message;

		    		std::cout << "\n\nAuth failed: " << message << std::endl;
		    	}
			    else if (static_cast<NetworkEvents>(eventCode) ==
			             NetworkEvents::SUCCESSFUL_AUTH)
			    {
				    std::cout << "\n\nWe in bois" << std::endl;
			    }
		    }
		    break;
		    case NetworkChannels::STATE:
			    break;
		    case NetworkChannels::VOXEL_DATA:
			    break;
		    case NetworkChannels::MESSAGE:
			    break;
		    case NetworkChannels::COUNT:
			    break;
		    default:
			    break;
		    }
	    });

	bool disconnected = false;
	host.onDisconnect([&disconnected](std::size_t peer, enet_uint32) {
		disconnected = true;
		std::cout << "\nDisconnected" << std::endl;
	});

	// we are going to get the server info.
	std::cout << "///////////////////////////// GETTING SERVER INFO "
	             "////////////////////" << std::endl;
	
	// we only connect with one channel, we ask for GET_SERVER_INFO.
	const std::string ip = "127.0.0.1";
	host.connect({ip, 7777}, static_cast<std::size_t>(NetworkChannels::GENERAL),
	             static_cast<enet_uint32>(NetworkEvents::GET_SERVER_INFO));

	int i = 0;
	while (!connected && i < 5)
	{
		host.poll(50_ms);
		++i;
	}

	if (!connected)
	{
		std::cout << "Could not connect to server. \n";
		exit(EXIT_FAILURE);
	}

	connected = false;

	while (!disconnected)
	{
		std::this_thread::sleep_for(20_ms);
		host.poll(50_ms);
	}
	
	std::string username = "toby";
	host.onConnect([&username, &connected](net::Peer& peer, enet_uint32 data) {
		connected = true;
		
		// we're connected in Connect mode now, so lets send over auth stuff.
		Serializer ser;
		ser << username;

		net::Packet packet(ser.getBuffer(), net::PacketFlags::RELIABLE);
		peer.send(packet, static_cast<enet_uint8>(NetworkChannels::AUTH));

		std::cout << "\n\n Attempting authentication with username: " << username;
	});
	
	// now we connect with every channel
	host.connect({ip, 7777}, static_cast<std::size_t>(NetworkChannels::COUNT),
	             static_cast<enet_uint32>(NetworkEvents::CONNECT));

	// we don't need to overwrite the onReceive, we just implemented it above.
	i = 0;
	while (!connected && i < 5)
	{
		host.poll(50_ms);
		++i;
	}
	
	if (!connected)
	{
		std::cout << "Could not connect to server. \n";
		exit(EXIT_FAILURE);
	}

	bool        running = true;
	std::thread t1([&running, &host]() {
		while (running)
		{
			host.poll(50_ms);
		}
	});

	std::string input;
	while (running)
	{
		std::cout << ">> ";
		std::cin >> input;
		if (input == "quit")
		{
			running = false;
		}
	}

	if (t1.joinable())
	{
		t1.join();
	}

	net::Network::teardown();

	return 0;
}
