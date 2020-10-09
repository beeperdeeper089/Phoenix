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

#include <Common/CMS/Mod.hpp>
#include <Common/CMS/ModManager.hpp>
#include <Common/Logger.hpp>

#include <Common/Math/Math.hpp>

using namespace phx::cms;

static void CustomPanicHandler(sol::optional<std::string> maybe_msg)
{
	// \n for newline, \t for tab.
	std::string error;
	if (maybe_msg)
	{
		error += "\n\t";
		error += maybe_msg.value();
		error += "\n";
	}

	LOG_FATAL("MODDING") << "An unexpected Lua error has occured. " << error
	                     << "The application will now be aborted.";
}

ModManager::ModManager()
{
	m_luaState.open_libraries(sol::lib::base);
	m_luaState.set_panic(
	    sol::c_call<decltype(&CustomPanicHandler), &CustomPanicHandler>);
}

ModManager::Status ModManager::load(const ModList& toLoad, const ModList& paths,
                                    float* progress)
{
	m_modsRequired = toLoad;
	m_modPaths     = paths;

	std::queue<Mod> loadOrder;

	*progress = 0.f;

	// we make 10% of the mod loading system finding the mods.
	float progressInterval = 0.1f / static_cast<float>(m_modsRequired.size());

	for (auto& require : m_modsRequired)
	{
		bool found = false;
		for (auto& path : m_modPaths)
		{
			std::fstream file;
			file.open(path + "/" + require + "/Init.lua");
			if (file.is_open())
			{
				// the mod is found, break out of the loop and a bit to the
				// progress.

				found = true;
				loadOrder.emplace(require, path);

				*progress += progressInterval;

				break;
			}
		}

		if (!found)
		{
			// the mod was not found in ANY of the directories.

			// make a list of the paths in a string.
			std::string pathList = "\n\t";
			for (auto& path : m_modPaths)
			{
				pathList += path;
				pathList += "\n\t";
			}

			LOG_FATAL("MODDING")
			    << "The mod: " << require
			    << " was not found in any of the provided mod directories: "
			    << pathList;

			return {false, "The mod: " + require + " was not found."};
		}
	}

	// we make 90% of the mod loading actually... loading the mods.
	progressInterval = 0.9f / static_cast<float>(m_modsRequired.size());

	std::vector<std::string> loadedMods;
	while (!loadOrder.empty())
	{
		std::size_t lastPass = loadOrder.size();

		for (std::size_t i = 0; i < loadOrder.size(); ++i)
		{
			Mod& mod       = loadOrder.front();
			bool satisfied = true;

			for (const auto& dependency : mod.getDependencies())
			{
				if (std::find(loadedMods.begin(), loadedMods.end(),
				              dependency) == loadedMods.end())
				{
					satisfied = false;
				}
			}

			if (satisfied)
			{
				// all the dependencies are satisfied, lets load the mod.
				m_currentModPath = mod.getPath() + "/" + mod.getName() + "/";
				sol::protected_function_result pfr =
				    m_luaState.safe_script_file(m_currentModPath + "Init.lua",
				                                &sol::script_pass_on_error);

				// error occured if return is not valid.
				if (!pfr.valid())
				{
					sol::error err = pfr;

					std::string errString = "An error occured loading ";
					errString += mod.getName();
					errString += ": ";
					errString += err.what();

					LOG_FATAL("MODDING") << errString;

					return {false, errString};
				}

				loadedMods.push_back(mod.getName());
				*progress += progressInterval;
			}
			else
			{
				loadOrder.push(loadOrder.front());
			}

			// remove the front mod, since if it's loaded, it doesn't need to
			// exist, and if it still needs dependencies, it will have been
			// pushed to the back.
			loadOrder.pop();
		}

		if (lastPass == loadOrder.size())
		{
			std::string err = "The mod: ";
			err += loadOrder.front().getName();
			err += " is missing one or more dependencies, please resolve this "
			       "issue before continuing.";

			LOG_FATAL("MODDING") << err;

			return {false, err};
		}
	}

	// rounding might be a bitch, so lets explicitly set progress to 1.
	*progress = 1.f;

	// no need to put in something for "what", since nothing went wrong.
	return {true};
}

void ModManager::cleanup() {}

const ModManager::ModList& ModManager::getModList() const
{
	return m_modsRequired;
}

const std::string& ModManager::getCurrentModPath() const
{
	return m_currentModPath;
}
