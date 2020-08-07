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

#include <Server/Server.hpp>

#include <cstring>

using namespace phx;

#undef main
int main(int argc, char** argv)
{
	// this may not be a smart way to parse CLI arguments, but it's supposed to
	// just *work* right now, we should use a tested library or make one
	// ourselves down the line.

	std::unordered_map<std::string, std::string> cliArguments;

	// we start from 1 because argv[0] will be the command used to run the
	// application. we iterate every 2 since you have the --command and the
	// Argument part.

	constexpr char doubleStroke[] = "--";

	int index = 1;
	while (index < argc)
	{
		// we will only implement the save and config file argument for now,
		// other single argument values can be used. for example, you have to do
		// --save Save1 or --config Settings.json, otherwise you will have to
		// just do --enableXXXXXX or something and it will be entered as a key
		// into the hash map. This can be helpful in testing.

		if (strncmp(argv[index], doubleStroke, 2) == 0)
		{
			// we are parsing an argument like --command.
			// lets drop the -- and use it as a key.

			const std::size_t keyLength = strlen(argv[index]);
			std::string       key(argv[index] + 2, keyLength - 2);

			if (key == "save")
			{
				// we need another argument with save.
				if (index + 1 < argc)
				{
					// lets put this value into the cliArguments.
					cliArguments[key] = argv[index + 1];
					index += 2;
				}
				else
				{
					// invalid argument list.
					// haven't bothered initializing logger yet, std::cout is
					// just fine for this.
					std::cout << "Incorrect Usage: the --save argument must be "
					             "followed by a save name."
					          << "\n\n"
					          << "Example Usage: ./PhoenixServer --save Save1 "
					             "--config Settings.json --enableXXXXX "
					             "--enableOtherFeature"
					          << std::endl;
					exit(EXIT_FAILURE);
				}
			}
			else if (key == "config")
			{
				// we need another argument with config.
				if (index + 1 < argc)
				{
					// lets put this value into the cliArguments.
					cliArguments[key] = argv[index + 1];
					index += 2;
				}
				else
				{
					// invalid argument list.
					// haven't bothered initializing logger yet, std::cout is
					// just fine for this.
					std::cout
					    << "Incorrect Usage: the --config argument must be "
					       "followed by a name for a configuration file."
					    << "\n\n"
					    << "Example Usage: ./PhoenixServer --save Save1 "
					       "--config Settings.json --enableXXXXX "
					       "--enableOtherFeature"
					    << std::endl;
					exit(EXIT_FAILURE);
				}
			}
			else if (key == "mods")
			{
				// we need to increment the index since we wanna check the next
				// one.
				++index;

				if (!(index < argc))
				{
					// index is NOT less than argc, there aren't any more
					// arguments, lets leave and say you are an idiot.
					std::cout << "Incorrect Usage: the --mods argument must be "
					             "followed by names of mods."
					          << "\n\n"
					          << "Example Usage: ./PhoenixServer --save Save1 "
					             "--config Settings.json --mods mod1 mod2 mod3"
					          << std::endl;
					exit(EXIT_FAILURE);
				}

				// this is hacky, but it works, fuck off.
				std::string modList;

				// we expect at least 1 mod and this makes the math work, we
				// have a fail safe if no mod exists.
				while (index < argc)
				{
					// every argument till the next -- is a mod.

					if (strncmp(argv[index], doubleStroke, 2) == 0)
					{
						// we've reached the next --, break out of this loop.
						break;
					}

					// push to a semi-colon delimited list.
					std::string mod = argv[index];
					modList += mod + ";";

					++index;
				}

				cliArguments[key] = modList;
			}
			else
			{
				// just insert the key, just so the key acts as a flag (could
				// help in testing).
				cliArguments[key] = "";
				index++;
			}
		}
		else
		{
			std::cout << "Incorrect usage through: " << argv[index] << "\n\n";

			std::cout << "Incorrect Usage: all command line arguments must "
			             "start with --, not just - or anything else."
			          << "\n\n"
			          << "Example Usage: ./PhoenixServer --save Save1 --config "
			             "Settings.json --enableXXXXX --enableOtherFeature"
			          << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	server::Server* server = new server::Server(cliArguments);
	server->run();
	delete server;
	
	return 0;
}
