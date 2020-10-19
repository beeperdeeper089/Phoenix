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

#include <Common/Game/Content/ModManager.hpp>
#include <Common/Logger.hpp>

#include <filesystem>
#include <queue>

using namespace phx::game;

static void CustomPanicHandler(sol::optional<std::string> maybeMsg)
{
	std::string error;
	if (maybeMsg)
	{
		error += "\n\t";
		error += maybeMsg.value();
		error += "\n";
	}

	LOG_FATAL("MODDING") << "An unexpected Lua error has occured. " << error
	                     << "The application will now be aborted.";
}

static int CustomExceptionHandler(lua_State* L, sol::optional<const std::exception&> maybeException, sol::string_view description)
{
	// https://sol2.readthedocs.io/en/latest/exceptions.html
	// i need to do more reading/testing on this - vyom.
	
	LOG_FATAL("MODDING") << "An exception occured during in a mod function.";
	if (maybeException)
	{
		LOG_FATAL("MODDING") << "The error: \n\t" << (*maybeException).what();
	}
	else
	{
		LOG_FATAL("MODDING")
		    << "The error: \n\t"
		    << std::string(description.data(), description.size());
	}

	return sol::stack::push(L, description);
}

ModManager::ModManager()
{
	m_state.open_libraries(sol::lib::base);
	m_state.set_panic(
	    sol::c_call<decltype(&CustomPanicHandler), &CustomPanicHandler>);
	m_state.set_exception_handler(&CustomExceptionHandler);
}

void ModManager::setup(const ModList& mods, const ModList& paths)
{
	m_mods = mods;
	m_paths = paths;
}

bool ModManager::validate()
{
	namespace fs = std::filesystem;
	
	// first check whether we have any mods.
	if (m_mods.empty())
	{
		// nothing can go wrong if we don't have any mods.
		return true;
	}

	// check to make sure all paths exist.
	for (auto& path : m_paths)
	{
		if (!fs::exists(path) || !fs::is_directory(path))
		{
			m_errorState = State::ERRORED;
			m_error      = "The provided mod search directory: " + path +
			          " does not exist.";
			
			return false;
		}
	}

	// check to make sure all mods exist.
	std::queue<Mod> checked;
	for (auto& name : m_mods)
	{
		bool found = false;

		for (auto& path : m_paths)
		{
			Mod mod(name, path);
			if (mod.valid())
			{
				found = true;
				checked.emplace(mod);
				break;
			}
		}

		if (!found)
		{
			// mod was not found in any directories.
			std::string pathList = "\n\t";
			for (auto& path : m_paths)
			{
				pathList += path;
				pathList += "\n\t";
			}

			m_errorState = State::ERRORED;
			m_error = "The mod: " + name +
			          " was not found in any provided directory.";

			return false;
		}
	}

	// make sure every mod has it's dependency.
	// also push to load order.
	while (!checked.empty())
	{
		const std::size_t lastPass = checked.size();

		for (std::size_t i = 0; i < checked.size(); ++i)
		{
			Mod& mod       = checked.front();
			bool satisfied = true;

			for (const auto& dependency : mod.getDependencies())
			{
				if (std::find(m_mods.begin(), m_mods.end(), dependency.name) == m_mods.end())
				{
					// dep doesn't exist.

					// dep is optional, so just continue with the next
					// dependency.
					if (dependency.optional)
					{
						continue;
					}

					satisfied = false;

					// no point iterating longer if one mod is dissatisfied.
					break;
				}
			}

			if (satisfied)
			{
				m_loadOrder.push(checked.front());
			}
			else
			{
				checked.push(checked.front());
			}

			checked.pop();
		}

		if (lastPass == checked.size())
		{
			m_errorState = State::ERRORED;
			m_error      = "The mod: ";
			m_error += checked.front().getName();
			m_error += " has one or more missing dependencies.";

			return false;
		}
	}

	// everything is good, tell the caller that all's good.
	return true;
}

bool ModManager::load(float* progress)
{
	namespace fs = std::filesystem;

	// load all mods. quit on error.
	while (!m_loadOrder.empty())
	{
		Mod& mod = m_loadOrder.front();
		
		fs::path initLoc = fs::path(mod.getPath()) / mod.getName() / "Init.lua";

		sol::protected_function_result pfr = m_state.safe_script_file(
		    initLoc.string(), &sol::script_pass_on_error);

		if (!pfr.valid())
		{
			sol::error err = pfr;

			m_errorState = State::ERRORED;
			m_error      = "An error occured loading ";
			m_error += mod.getName();
			m_error += ": ";
			m_error += err.what();

			return false;
		}

		m_loadOrder.pop();
	}

	// to anyone reading this: if we need to keep these, just delete the
	// respective lines of code, clearing this is not essential, it just makes
	// sense since as of right now, we don't need them at all anymore.
	// the active, loaded mods can be requested via getActiveMods.
	// we've loaded all the mods, now we can just clear m_mods.
	m_mods.clear();
	// we can also clear m_paths.
	m_paths.clear();
	
	// all went good, lets go!
	return true;
}

// can throw but it really shouldn't if the loading logic is correct.
const Mod& ModManager::getActiveMod() const { return m_loadOrder.front(); }

const std::vector<Mod>& ModManager::getActiveMods() const
{
	return m_loadedMods;
}

std::string ModManager::getError() { return m_error; }

sol::state_view ModManager::getUnderlyingState()
{
	// i'm not sure whether we can return m_state or if we have to return the
	// direct lua state.
	return m_state.lua_state();
}