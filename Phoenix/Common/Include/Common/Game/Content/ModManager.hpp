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

#include <Common/Logger.hpp>
#include <Common/Game/Content/Mod.hpp>

#include <sol/sol.hpp>

#include <string>
#include <vector>
#include <queue>

namespace phx::game
{
	class ModManager
	{
	public:
		enum class State
		{
			WORKING,
			ERRORED
		};
		
		using ModList = std::vector<std::string>;

	public:
		ModManager();

		void        setup(const ModList& mods, const ModList& paths);
		bool        validate();
		bool        load(float* progress);
		const Mod&  getActiveMod() const;

		const std::vector<Mod>& getActiveMods() const;
		
		std::string getError();

		template <typename T>
		void registerFunction(const std::string& funcName, const T& func);

		sol::state_view getUnderlyingState();
		
	private:
		State       m_errorState = State::WORKING;
		std::string m_error;
		
		ModList m_mods;
		ModList m_paths;

		std::queue<Mod>  m_loadOrder;
		std::vector<Mod> m_loadedMods;
		
		sol::state m_state;
	};
} // namespace phx::game

#include <Common/Game/Content/ModManager.inl>
