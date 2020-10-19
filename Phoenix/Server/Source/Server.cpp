// Copyright 2020 Genten Studios
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

#include <Common/Logger.hpp>
#include <Common/Settings.hpp>

#include <Server/Server.hpp>

using namespace phx::server;

void registerIntegralAPI(phx::cms::ModManager);
void registerUnusedAPI(phx::cms::ModManager);

Server::Server(const std::unordered_map<std::string, std::string>& cliArguments)
    : m_arguments(cliArguments)
{}

void Server::run()
{
	//// STARTUP ////

	// LOADING SETTINGS

	const auto configIterator = m_arguments.find("config");
	if (configIterator == m_arguments.end())
	{
		configIterator->second = "Settings.json";
	}

	Settings::get()->load(configIterator->second);

	// INITIALIZING LOGGER
	
	const auto* logVerb =
	    Settings::get()->add("Log Verbosity", "core:log_verbosity",
	                         static_cast<int>(LogVerbosity::INFO));

	LoggerConfig logConfig;
	logConfig.verbosity = static_cast<LogVerbosity>(logVerb->value());
	Logger::initialize(logConfig);

	// LOADING SAVE.
	
	const auto saveIterator = m_arguments.find("save");
	if (saveIterator == m_arguments.end())
	{
		LOG_INFO("SERVER") << "No save file specified, loading default World.";
		saveIterator->second = "World";
	}

	const auto& saves = Save::listAllSaves();
	if (std::find(saves.begin(), saves.end(), saveIterator->second) == saves.end())
	{
		LOG_INFO("SERVER") << "Save does not exist, creating...";
		
		std::vector<std::string> modList;
		const auto               modIterator = m_arguments.find("mods");
		if (modIterator != m_arguments.end())
		{
			// @todo Not memory efficient, creating copies. Optimise.
			std::stringstream modString(modIterator->second);
			std::string       mod;
			while (std::getline(modString, mod, ';'))
			{
				modList.emplace_back(mod);
			}
		}

		m_save = new Save(saveIterator->second, modList);
	}
	else
	{
		LOG_INFO("SERVER") << "Loading existing save...";
		m_save = new Save(saveIterator->second);
	}

	// INITIALIZE NETWORKING.
}
