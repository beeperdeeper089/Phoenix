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

#include <Common/Game/Content/Mod.hpp>
#include <Common/Logger.hpp>

#include <filesystem>

using namespace phx::game;

Mod::Mod(const std::string& name, const std::string& folder)
    : m_name(name), m_path(folder)
{
	{
		namespace fs = std::filesystem;
		
		fs::path modDir = fs::path(m_path) / m_name;
		if (fs::exists(modDir) && fs::is_directory(modDir))
		{
			// folder exists, lets see what we can do now.
			if (!fs::exists(modDir / "Init.lua"))
			{
				// Init.lua does not exist, not a valid mod.
				LOG_FATAL("MODDING")
				    << "The mod: " << m_name << "at: " << m_path
				    << " is invalid. It does not contain an Init.lua";

				// nothing more should be done, return here.
				return;
			}

			// Init.lua exists, so far this is a valid mod in our eyes.
			// now check if there are any dependencies.
			if (fs::exists(modDir / "Dependencies.txt"))
			{
				std::fstream deps(modDir / "Dependencies.txt");
				if (deps.is_open())
				{
					// there is a dependencies file. parse it.
					// new mod per newline. ! at the front means optional, otherwise it is required.
					std::string input;
					while (std::getline(deps, input))
					{
						if (!input.empty())
						{
							Dependency dep;

							if (input[0] == '!')
							{
								// dependency is optional, strip first character since that is a !.
								dep.name     = input.substr(1);
								dep.optional = true;

								// testing.
								LOG_DEBUG("MODDING")
								    << "Optional Mod: " << input.substr(1);
							}
							else
							{
								// dependency is not optional, move the whole string over.
								dep.name = std::move(input);
							}

							m_dependencies.emplace_back(std::move(dep));
						}
					}
				}
			}
		}
		else
		{
			LOG_FATAL("MODDING")
			    << "The mod: " << m_name
			    << " was not found at the requested location: " << m_path;

			// nothing more should be done, return here.
			return;
		}
	}
}

Mod::~Mod()
{
}

const std::string& Mod::getName() const { return m_name; }

const std::string& Mod::getPath() const { return m_path; }

const std::vector<Mod::Dependency>& Mod::getDependencies() const
{
	return m_dependencies;
}
