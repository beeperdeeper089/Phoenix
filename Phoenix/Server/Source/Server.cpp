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

void registerUnusedAPI(cms::ModManager* manager)
{
	manager->registerFunction("core.input.registerInput",
	                          [](std::string uniqueName,
	                             std::string displayName,
	                             std::string defaultKey) {});
	manager->registerFunction("core.input.getInput", [](int input) {});
	manager->registerFunction("core.input.getInputRef",
	                          [](std::string uniqueName) {});
	manager->registerFunction("core.input.registerCallback",
	                          [](int input, sol::function f) {});
	manager->registerFunction(
	    "audio.loadMP3",
	    [=](const std::string& uniqueName, const std::string& filePath) {});
	manager->registerFunction("audio.play", [=](sol::table source) {});
}

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

	m_modManager = new cms::ModManager(m_save->getModList(), {"Modules"});

	m_modManager->registerFunction("core.print", [=](const std::string& text) {
		std::cout << text << "\n";
	});

	LoggerConfig logConfig;
	logConfig.verbosity = static_cast<LogVerbosity>(logVerb->value());
	Logger::initialize(logConfig);

	m_modManager->registerFunction("core.log_warning", [](std::string message) {
		LOG_WARNING("MODULE") << message;
	});
	m_modManager->registerFunction("core.log_fatal", [](std::string message) {
		LOG_FATAL("MODULE") << message;
	});
	m_modManager->registerFunction("core.log_info", [](std::string message) {
		LOG_INFO("MODULE") << message;
	});
	m_modManager->registerFunction("core.log_debug", [](std::string message) {
		LOG_DEBUG("MODULE") << message;
	});

	float progress = 0.f;
	auto  result   = m_modManager->load(&progress);

	if (!result.ok)
	{
		LOG_FATAL("CMS") << "An error has occurred loading modules.";
		exit(EXIT_FAILURE);
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
