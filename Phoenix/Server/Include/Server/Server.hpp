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

#include <Common/Save.hpp>
#include <Server/Voxels/BlockRegistry.hpp>
#include <Common/CMS/ModManager.hpp>
#include <Server/Iris.hpp>

#include <entt/entt.hpp>

#include <unordered_map>
#include <string>

namespace phx::server
{
	class Server
	{
	public:
		Server(const std::unordered_map<std::string, std::string>& cliArguments);
		~Server() = default;

		void run();

	private:
		/**
		 * @brief The boolean telling the server to run or not.
		 */
		bool m_running = true;

		/**
		 * @brief The arguments used when launching the server.
		 */
		std::unordered_map<std::string, std::string> m_arguments;

		/**
		 * @brief The save in use.
		 */
		Save m_save;

		/**
		 * @brief The mod loader/handler.
		 *
		 * This needs to be above anything that stores anything to do with lua
		 * (even a sol::function) since the order of destruction means that
		 * those will be destroyed before the state is.
		 */
		cms::ModManager m_modManager;

		/**
		 * @brief The block registry the server will append to.
		 */
		BlockRegistry m_blockRegistry;
		
		/**
		 * @brief The ECS Registry in which entities and components will reside.
		 */
		entt::registry m_registry;

		/**
		 * @brief The Network handler for the server.
		 */
		Iris* m_iris = nullptr;

		/**
		 * @brief The server side game object, this handles all of the core game
		 * logic.
		 */
		//Game* m_game;


	};
} // namespace phx::server
