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
#include <Client/Game/MainMenu.hpp>
#include <Client/Graphics/Tooling/Timer.hpp>

#include <Common/Logger.hpp>
#include <Common/Settings.hpp>

using namespace phx::client;

void Client::initialize(const std::unordered_map<std::string, std::vector<std::string>>& cliArguments)
{
	//// STARTUP ////

	// LOADING SETTINGS

	m_arguments = cliArguments;
	
	auto configIterator = m_arguments.find("config");
	if (configIterator == m_arguments.end())
	{
		configIterator =
		    m_arguments
		        .emplace("config", std::vector<std::string>{"Settings.json"})
		        .first;
	}

	Settings::get()->load(configIterator->second[0]);

	// INITIALIZING LOGGER

	const auto* logVerb =
	    Settings::get()->add("Log Verbosity", "core.log_verbosity",
	                         static_cast<int>(LogVerbosity::INFO));

	LoggerConfig logConfig;
	logConfig.verbosity = static_cast<LogVerbosity>(logVerb->value());
	Logger::initialize(logConfig);

	// INITIALIZE WINDOW
	m_window = new phx::gfx::Window("Phoenix", 1280, 720);
	m_window->registerEventListener(this);

	m_layerStack = new phx::gfx::LayerStack(m_window);
	
	// READY TO RUN.
}

void Client::teardown()
{
	// as long as initialize has been called and the config variable has been
	// set, we shouldn't have an issue at all.
	Settings::get()->save(m_arguments.at("config")[0]);
}

void Client::pushLayer(phx::gfx::Layer* layer)
{
	if (layer->isOverlay())
	{
		m_layerStack->pushOverlay(layer);
	}
	else
	{
		m_layerStack->pushLayer(layer);
	}
}

void Client::popLayer(phx::gfx::Layer* layer)
{
	if (layer->isOverlay())
	{
		m_layerStack->popOverlay(layer);
	}
	else
	{
		m_layerStack->popLayer(layer);
	}
}

void Client::onEvent(phx::events::Event e)
{
	using namespace events;
	switch (e.type)
	{
	case EventType::KEY_PRESSED:
		switch (e.keyboard.key)
		{
		case Keys::KEY_F3:
			// enable debug overlay.
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	if (!e.handled)
	{
		m_layerStack->onEvent(e);
	}
}

void Client::run()
{
	MainMenu* menu = new MainMenu(m_window, m_arguments);
	m_layerStack->pushLayer(menu);
	
	phx::gfx::Timer timer;
	while (m_window->isRunning())
	{		
		m_window->startFrame();

		if (!m_layerStack->empty())
			m_layerStack->tick(timer.step());

		m_window->endFrame();
	}
}
