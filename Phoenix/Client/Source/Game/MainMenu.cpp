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

#include <Client/Game/MainMenu.hpp>

#include <Common/Game/Save.hpp>

#include <imgui.h>
#include <cstring>

using namespace phx::client;

MainMenu::MainMenu(gfx::Window*                                  window,
                   std::unordered_map<std::string, std::vector<std::string>>& cliArguments)
    : Layer("MainMenu"), m_arguments(cliArguments), m_window(window)
{
	m_addressBuffer = std::make_unique<char>(255);
	strcpy(m_addressBuffer.get(), "127.0.0.1");

	auto pathIterator = m_arguments.find("paths");
	if (pathIterator == m_arguments.end())
	{
		// if nothing exists, force at least Modules/
		pathIterator = m_arguments.emplace("paths", std::vector<std::string>{"Modules"}).first;
	}
}

MainMenu::~MainMenu() {}

void MainMenu::onAttach()
{
	m_installedMods = phx::game::ModManager::getAllInstalledMods(m_arguments["paths"]);
	m_saves = phx::Save::listAllSaves();
}

void MainMenu::onDetach() {}

void MainMenu::onEvent(events::Event& e)
{
}

void MainMenu::tick(float dt)
{
	ImGui::Begin("Network");
	{
		ImGui::Text("Address:");
		ImGui::SameLine();
		ImGui::InputText("", m_addressBuffer.get(), 255);
		ImGui::SameLine();

		if (ImGui::Button("Connect"))
		{
			
		}
	}
	ImGui::End();

	ImGui::Begin("Installed Mods");
	{
		if (!m_installedMods.empty())
		{
			for (auto& mod : m_installedMods)
			{
				ImGui::BulletText(mod.c_str());
			}
		}
		else
		{
			ImGui::Text("No mods found.");
		}
		
	}
	ImGui::End();

	ImGui::Begin("Saves");
	{
		if (!m_saves.empty())
		{
			for (auto& save : m_saves)
			{
				ImGui::BulletText(save.c_str());
			}
		}
		else
		{
			ImGui::Text("No existing saves found.");
		}
		
	}
	ImGui::End();
}
