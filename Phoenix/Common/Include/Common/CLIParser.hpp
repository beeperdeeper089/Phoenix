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

#include <string>
#include <vector>
#include <unordered_map>

namespace phx
{
	struct CLIParameter
	{
		// the full name of the parameter to read from the CLI, must be prefixed with --
		std::string parameter;

		// the shorthand of the parameter to read from the CLI, must be prefixed with -.
		std::string shorthand;

		// the help string of the parameter. will be printed when --help is used.
		// you can do --help full_name_of_parameter to get just this message.
		std::string helpString;

		// try not to make any parameters required, but the feature's here.
		bool required = false;
		// read multiple values for this, otherwise ignore and error if there are multiple parameters.
		bool multiValue = false;
		// if this is a flag, it will not have any values, it will just exist.
		bool isFlag     = false;
	};

	/**
	 * @brief Parses any CLI arguments provided.
	 *
	 * This class will parse all registered parameters when the parse function
	 * is called from the main method using the argc and argv arguments.
	 *
	 * @paragraph Terminology
	 *     Parameter: The actual name/details of the flag used on the command line.
	 *     Argument: The actual value given and parsed.
	 */
	class CLIParser
	{
	public:
		CLIParser();
		~CLIParser();

		void addParameter(const CLIParameter& parameter);
		std::vector<CLIParameter*> getParameterList() const;

		void parse(int argc, char** argv);

		std::vector<std::string>* getArgument(
		    const std::string& parameter) const;

	private:
		std::vector<CLIParameter>                    m_parameters;
		std::unordered_map<std::string, std::size_t> m_fullParamMap;
		std::unordered_map<std::string, std::size_t> m_shorthandMap;
		std::unordered_map<std::string, std::vector<std::string>> m_arguments;
	};
}
