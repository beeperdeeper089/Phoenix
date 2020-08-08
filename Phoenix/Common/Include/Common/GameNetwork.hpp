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

namespace phx
{
	enum class NetworkChannels : unsigned char
	{
		// get server info, connect, refusal messages, other general messages.
		GENERAL = 0,

		// literally, authentication. we won't do passwords and all yet since we don't have encryption.
		AUTH,

		// states.
		STATE,

		// chunk data, entity data, registry syncing, etc...
		VOXEL_DATA,

		// chat messages (includes commands).
		MESSAGE,

		// just the count of all the channels we need, makes it easier to expand.
		COUNT
	};
	
	enum class NetworkEvents
	{
		// start at one so we know 0 is always nothing from the extra piece of
		// data that enet can handle on connect and disconnect.
		
		// for use in the General channel.
		GET_SERVER_INFO = 1,
		CONNECT,
		GRACEFUL_DISCONNECT,
		KICK,

		// for use in the Auth channel.
		INVALID_AUTH,
		SUCCESSFUL_AUTH,
		
		// for use in the VoxelData channel.
		NEW_CHUNK,
		CHUNK_UPDATE,
	};
} // namespace phx
