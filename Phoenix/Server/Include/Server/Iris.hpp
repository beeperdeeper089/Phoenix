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

#pragma once

// This is needed because Windows https://github.com/skypjack/entt/issues/96
#ifndef NOMINMAX
#	define NOMINMAX
#endif

#include <Common/Settings.hpp>
#include <Common/Network/Host.hpp>
#include <Common/Player.hpp>

#include <Common/Voxels/Chunk.hpp>

#include <Common/Input.hpp>
#include <Common/Util/BlockingQueue.hpp>

#include <entt/entt.hpp>

namespace phx::server
{
	// struct StateBundle
	//{
	//	bool                                         ready;
	//	std::size_t                                  users;
	//	std::size_t                                  sequence;
	//	std::unordered_map<entt::entity, InputState> states;
	//};

	// struct MessageBundle
	//{
	//	std::size_t userID;
	//	std::string message;
	//};

	// struct Event
	//{
	//	enum class Type
	//	{
	//		CONNECT
	//	};
	//	entt::entity player;
	//	Type         type;
	//};

	class Iris
	{
	public:
		explicit Iris(entt::registry* registry);
		~Iris();

		void run();
		void kill();
		
		void onConnect(net::Peer& peer, enet_uint32 reason);
		void onDisconnect(std::size_t peer, enet_uint32 reason);

		void authenticateClient(net::Peer& peer, net::Packet&& packet);

		// void parseEvent(std::size_t userID, phx::net::Packet& packet);
		// void parseState(std::size_t userID, phx::net::Packet& packet);
		// void parseMessage(std::size_t userID, phx::net::Packet& packet);

		// void sendEvent(std::size_t userID, enet_uint8* data);
		// void sendState(entt::registry* registry, std::size_t sequence);
		// void sendMessage(std::size_t userID, const std::string& message);
		// void sendData(std::size_t userID, voxels::Chunk* data);

		// BlockingQueue<Event> eventQueue;

		// std::vector<StateBundle>   currentBundles;
		// BlockingQueue<StateBundle> stateQueue;

		// BlockingQueue<MessageBundle> messageQueue;

	private:
		struct ConnectingPlayer
		{
			std::size_t peerID;
			std::size_t timeSince;
		};
		
	private:
		bool m_running = false;

		Setting* m_maxClients = nullptr;
		
		net::Host*  m_server;
		std::string m_serverName;
		std::string m_serverMessage;

		entt::registry* m_registry;

		// this is for players if they are connecting before they have finished
		// "authing", and if they are in there over a specific time, we dip them.
		std::list<std::size_t> m_connectingPlayers;
		
		std::unordered_map<std::size_t, Player> m_users;
	};
} // namespace phx::server
