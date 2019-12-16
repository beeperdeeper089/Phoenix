// Copyright 2019 Genten Studios
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

#include <Quartz2/ContentLoader.hpp>

using namespace q2;

Mod::Mod(std::string name) : m_name(std::move(name))
{
	std::fstream fileStream;
	fileStream.open("Modules/" + name + "/dependencies.txt");
	while (fileStream.peek() != EOF)
	{
		std::string input;
		std::getline(fileStream, input);
		m_dependencies.push_back(input);
	}
	fileStream.close();
};
Mod::~Mod() {};

bool Mod::exists()
{
	return true; // TODO make this actually check for mod
}

bool modules::loadModules(std::string save, sol::state& lua)
{
	std::fstream fileStream;
	std::queue<Mod> toLoad; // A queue of mods that need loaded

	fileStream.open("Save/" + save + "/mods.txt");
	int i = 0;
	while (fileStream.peek() != EOF)
	{
		std::string input;
		std::getline(fileStream, input);
		Mod mod = Mod(input);
		if (!mod.exists())
		{
			std::cout << "Mod does not exist" + mod.m_name;
			return false;
		}; // Should never happen if launcher does all the work but lets check
		   // anyways
		toLoad.push(mod);
		if (i > 10)
			break;
		i++;
	}
	fileStream.close();

	// Sort and load the mods
	std::vector<std::string> loadedMods;
	while (toLoad.size() > 0)
	{
		int lastPass = toLoad.size();
		// For each mod that needs loaded
		for (int i = 0; i < toLoad.size(); i++)
		{
			Mod  mod       = toLoad.front();
			bool satisfied = true;
			// For each dependency the mod has
			for (int j = 0; j < mod.m_dependencies.size(); j++)
			{
				// If dependency is not satisfied, mark satisfied as false.
				if (std::find(loadedMods.begin(), loadedMods.end(),
				              mod.m_dependencies[j]) == loadedMods.end())
				{
					satisfied = false;
				}
			}
			// If all dependencies were met, run lua and add mod to satisfied
			// list Otherwise, move mod to back of load queue
			if (satisfied)
			{
				lua.script_file("modules/" + mod.m_name + "/init.lua");
				loadedMods.push_back(mod.m_name);
			}
			else
			{
				toLoad.push(toLoad.front());
			}
			toLoad.pop();
		}
		// If we haven't loaded any new mods, throw error
		if (lastPass == toLoad.size())
		{
			// TODO: Replace this with actuall error handling/ logging
			std::cout
			    << "One or more mods are missing required dependencies \n";
			std::cout << "Failed Mods:\n";
			for (int i = 0; i < toLoad.size(); i++)
			{
				std::cout << "- " + toLoad.front().m_name + "\n";
				toLoad.pop();
			}
			std::cout << "Loaded Mods:\n";
			for (int i = 0; i < loadedMods.size(); i++)
			{
				std::cout << "- " + loadedMods[i] + "\n";
			}
			return false;
		}
	}

	return true;
}