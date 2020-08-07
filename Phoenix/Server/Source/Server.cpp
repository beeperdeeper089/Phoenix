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

#include <Server/Server.hpp>

#include <Common/Voxels/BlockReferrer.hpp>

#include <Common/Logger.hpp>
#include <Common/Settings.hpp>

#include <sstream>
#include <utility>

using namespace phx::server;

Server::Server(const std::unordered_map<std::string, std::string>& cliArguments)
    : m_arguments(cliArguments)
{
	// Load Settings.
	const auto configIterator = m_arguments.find("config");
	if (configIterator == m_arguments.end())
	{
		m_arguments["config"] = "Settings.json";
	}

	Settings::get()->load(m_arguments["config"]);

	// Initialize Logger.
	const auto* logVerb =
	    Settings::get()->add("Log Verbosity", "core:log_verbosity",
	                         static_cast<int>(LogVerbosity::INFO));

	LoggerConfig logConfig;
	logConfig.verbosity = static_cast<LogVerbosity>(logVerb->value());
	Logger::initialize(logConfig);

	// Load Save
	const auto saveIterator = m_arguments.find("save");
	if (saveIterator == m_arguments.end())
	{
		m_arguments["save"] = "World";
	}

	std::vector<std::string> modList;
	const auto               modIterator = m_arguments.find("mods");
	if (modIterator != m_arguments.end())
	{
		std::stringstream modString(modIterator->second);
		std::string       mod;
		while (std::getline(modString, mod, ';'))
		{
			modList.emplace_back(std::move(mod));
		}
	}

	// we're not doing anything with the settings variable, but we don't
	// actually use it yet so just let it be.
	m_save = Save(m_arguments["save"], modList);
}

// void registerUnusedAPI(cms::ModManager* manager)
//{
//	manager->registerFunction("core.input.registerInput",
//	                          [](std::string uniqueName,
//	                             std::string displayName,
//	                             std::string defaultKey) {});
//	manager->registerFunction("core.input.getInput", [](int input) {});
//	manager->registerFunction("core.input.getInputRef",
//	                          [](std::string uniqueName) {});
//	manager->registerFunction("core.input.registerCallback",
//	                          [](int input, sol::function f) {});
//	manager->registerFunction(
//	    "audio.loadMP3",
//	    [=](const std::string& uniqueName, const std::string& filePath) {});
//	manager->registerFunction("audio.play", [=](sol::table source) {});
//}

void Server::run() {}

Server::~Server()
{
	// force save on shutdown.
	m_save.toFile();

	Settings::get()->save(m_arguments["config"]);

	Logger::teardown();
}
