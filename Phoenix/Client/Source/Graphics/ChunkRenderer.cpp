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

#include <Client/Graphics/ChunkMesher.hpp>
#include <Client/Graphics/ChunkRenderer.hpp>
#include <Client/Graphics/OpenGLTools.hpp>

#include <Common/Actor.hpp>
#include <Common/PlayerView.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <stb_image.h>

#include <unordered_set>

using namespace phx::gfx;

ChunkRenderer::ChunkRenderer(phx::voxels::Map*           map,
                             phx::client::BlockRegistry* blockRegistry,
                             entt::registry* registry, entt::entity entity)
    : m_blockRegistry(blockRegistry), m_map(map), m_registry(registry),
      m_entity(entity)
{
	// lets say you have a view distance of 10, so lets do 10x10x10 and
	// just say you're gonna have 100 chunks in view at a time. you'll be
	// able to have more obviously, but this is just a simple
	// optimisation.
	m_chunks.reserve(100);
	m_buffers.reserve(100);

	glGenVertexArrays(1, &m_selectionBoxVAO);
	glBindVertexArray(m_selectionBoxVAO);
	glGenBuffers(1, &m_selectionBoxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_selectionBoxVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	m_selectionBoxPipeline.prepare("Assets/SimpleLines.vert",
	                               "Assets/SimpleLines.frag", {{"a_Pos", 0}});
}

ChunkRenderer::~ChunkRenderer() { clear(); }

std::vector<ShaderLayout> ChunkRenderer::getRequiredShaderLayout()
{
	std::vector<ShaderLayout> layout;
	layout.emplace_back("a_Vertex", 0);
	layout.emplace_back("a_UV", 1);

	return layout;
}

void ChunkRenderer::prep()
{
	m_blockRegistry->texturePacker.pack();
}

void ChunkRenderer::attachCamera(FPSCamera* camera) { m_camera = camera; }

void ChunkRenderer::add(phx::voxels::Chunk* chunk)
{
	const auto it = std::find(m_chunks.begin(), m_chunks.end(), chunk);
	if (it == m_chunks.end())
	{
		m_chunks.push_back(chunk);
	}
	else
	{
		// chunk already exists, lets do nothing.
		return;
	}

	auto mesh = ChunkMesher::mesh(chunk, m_blockRegistry);
	if (mesh.empty())
	{
		// the mesh is empty, don't bother with adding it or anything.
		return;
	}

	unsigned int vao;
	unsigned int buf;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size(), mesh.data(),
	             GL_DYNAMIC_DRAW);

	glVertexAttribPointer(m_vertexAttributeLocation, 3, GL_FLOAT, GL_FALSE,
	                      sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, pos)));

	// we're gonna pack texLayer into the UV struct cos why not.
	glVertexAttribPointer(m_uvAttributeLocation, 3, GL_FLOAT, GL_FALSE,
	                      sizeof(Vertex),
	                      reinterpret_cast<void*>(offsetof(Vertex, uv)));

	glVertexAttribPointer(
	    m_normalAttributeLocation, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex),
	    reinterpret_cast<void*>(offsetof(Vertex, normal)));

	glEnableVertexAttribArray(m_vertexAttributeLocation);
	glEnableVertexAttribArray(m_uvAttributeLocation);
	glEnableVertexAttribArray(m_normalAttributeLocation);

	m_buffers.insert({chunk->getChunkPos(), {vao, buf, mesh.size()}});
}

void ChunkRenderer::update(phx::voxels::Chunk* chunk)
{
	const auto it = std::find(m_chunks.begin(), m_chunks.end(), chunk);
	if (it == m_chunks.end())
	{
		// add a chunk if not exists for compatibility.
		add(chunk);
		return;
	}

	auto mesh = ChunkMesher::mesh(chunk, m_blockRegistry);

	// we can't just say return if the mesh is empty, since we might be emptying
	// a mesh (breaking the final block in a chunk so only air is left or
	// something)

	ChunkRenderData data;
	auto            bufferExist = m_buffers.find(chunk->getChunkPos());
	if (bufferExist == m_buffers.end())
	{
		// data does not exist on GPU, we gotta make it.
		// but check if the mesh is empty first.
		if (mesh.empty())
		{
			return;
		}

		glGenVertexArrays(1, &data.vao);
		glBindVertexArray(data.vao);

		glGenBuffers(1, &data.buffer);
		glBindBuffer(GL_ARRAY_BUFFER, data.buffer);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size(), mesh.data(),
		             GL_DYNAMIC_DRAW);

		glVertexAttribPointer(m_vertexAttributeLocation, 3, GL_FLOAT, GL_FALSE,
		                      sizeof(Vertex),
		                      reinterpret_cast<void*>(offsetof(Vertex, pos)));

		// we're gonna pack texLayer into the UV struct cos why not.
		glVertexAttribPointer(m_uvAttributeLocation, 3, GL_FLOAT, GL_FALSE,
		                      sizeof(Vertex),
		                      reinterpret_cast<void*>(offsetof(Vertex, uv)));

		glVertexAttribPointer(
		    m_normalAttributeLocation, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex),
		    reinterpret_cast<void*>(offsetof(Vertex, normal)));

		glEnableVertexAttribArray(m_vertexAttributeLocation);
		glEnableVertexAttribArray(m_uvAttributeLocation);
		glEnableVertexAttribArray(m_normalAttributeLocation);

		data.vertexCount = mesh.size();
		m_buffers.insert({chunk->getChunkPos(), data});
	}
	else
	{
		// buffer already exists.
		glBindVertexArray(bufferExist->second.vao);
		glBindBuffer(GL_ARRAY_BUFFER, bufferExist->second.buffer);

		if (mesh.size() == bufferExist->second.vertexCount)
		{
			// if the vertex count is the same, don't reallocate the buffer,
			// just change the value - will save expensive reallocation.
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * mesh.size(),
			                mesh.data());
		}
		else
		{
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size(),
			             mesh.data(), GL_DYNAMIC_DRAW);

			bufferExist->second.vertexCount = mesh.size();
		}
	}
}

void ChunkRenderer::remove(phx::voxels::Chunk* chunk)
{
	const auto it = std::find(m_chunks.begin(), m_chunks.end(), chunk);
	if (it != m_chunks.end())
	{
		// chunks is found, lets do something.

		// delete the opengl buffers.
		const auto buffer = m_buffers.find((*it)->getChunkPos());
		if (buffer != m_buffers.end())
		{
			glDeleteBuffers(1, &buffer->second.buffer);
			glDeleteVertexArrays(1, &buffer->second.vao);
		}

		// remove the buffer and chunk from internal memory.
		m_buffers.erase((*it)->getChunkPos());
		m_chunks.erase(it);
	}
}

void ChunkRenderer::clear()
{
	for (auto& buffer : m_buffers)
	{
		glDeleteBuffers(1, &buffer.second.buffer);
		glDeleteVertexArrays(1, &buffer.second.vao);
	}
}

void ChunkRenderer::onMapEvent(const phx::voxels::MapEvent& mapEvent)
{
	m_mapEvents.push(mapEvent);
}

void ChunkRenderer::tick(float dt)
{
	for (auto& chunk : PlayerView::update(m_registry, m_entity))
	{
		add(chunk);
	}

	voxels::MapEvent e;
	while (m_mapEvents.try_pop(e))
	{
		if (e.type == voxels::MapEvent::CHUNK_UPDATE)
		{
			update(e.chunk);
		}
	}
	
	m_blockRegistry->texturePacker.activate(0);

	for (auto& buffer : m_buffers)
	{
		glBindVertexArray(buffer.second.vao);
		glDrawArrays(GL_TRIANGLES, 0, buffer.second.vertexCount);
	}

	// we shouldn't render the selection box in here since you might be
	// spectating. the box should really be a thing the player stuff renders.
}

void ChunkRenderer::renderSelectionBox()
{
	if (m_camera == nullptr)
	{
		// return if no camera set.
		return;
	}

	auto pos =
	    ActorSystem::getTarget(m_registry, m_entity).getCurrentPosition();
	pos.floor();
	// do not waste cpu time if we aren't targeting a solid block
	if (m_registry->get<PlayerView>(m_entity).map->getBlockAt(pos)->category !=
	    voxels::BlockCategory::SOLID)
	{
		return;
	}

	// voxel position to camera position
	pos.x = (pos.x - 0.5f) * 2.f;
	pos.y = (pos.y - 0.5f) * 2.f;
	pos.z = (pos.z - 0.5f) * 2.f;

	/*
	       1 +--------+ 2
	        /|       /|
	       / |   3  / |
	    0 +--------+  |
	      |  |6    |  |
	      |  x-----|--+ 7
	      | /      | /
	      |/       |/
	    5 +--------+ 4
	 */

	const float more = 2.001f;
	const float less = 0.001f;

	float vertices[] = {pos.x + more, pos.y + more, pos.z - less, // 0-1
	                    pos.x - less, pos.y + more, pos.z - less,

	                    pos.x - less, pos.y + more, pos.z - less, // 1-2
	                    pos.x - less, pos.y + more, pos.z + more,

	                    pos.x - less, pos.y + more, pos.z + more, // 2-3
	                    pos.x + more, pos.y + more, pos.z + more,

	                    pos.x + more, pos.y + more, pos.z + more, // 3-4
	                    pos.x + more, pos.y - less, pos.z + more,

	                    pos.x + more, pos.y - less, pos.z + more, // 4-5
	                    pos.x + more, pos.y - less, pos.z - less,

	                    pos.x + more, pos.y - less, pos.z - less, // 5-6
	                    pos.x - less, pos.y - less, pos.z - less,

	                    pos.x - less, pos.y - less, pos.z - less, // 6-7
	                    pos.x - less, pos.y - less, pos.z + more,

	                    pos.x - less, pos.y - less, pos.z + more, // 7-4
	                    pos.x + more, pos.y - less, pos.z + more,

	                    pos.x - less, pos.y - less, pos.z + more, // 7-2
	                    pos.x - less, pos.y + more, pos.z + more,

	                    pos.x - less, pos.y + more, pos.z - less, // 1-6
	                    pos.x - less, pos.y - less, pos.z - less,

	                    pos.x + more, pos.y + more, pos.z - less, // 0-3
	                    pos.x + more, pos.y + more, pos.z + more,

	                    pos.x + more, pos.y + more, pos.z - less, // 0-5
	                    pos.x + more, pos.y - less, pos.z - less};

	glBindVertexArray(m_selectionBoxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_selectionBoxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	m_selectionBoxPipeline.activate();

	// @todo fix so we aren't calculating the same thing twice (once in the main
	// render loop and once more here).
	m_selectionBoxPipeline.setMatrix("u_view", m_camera->calculateViewMatrix());
	m_selectionBoxPipeline.setMatrix("u_projection", m_camera->getProjection());
	glDrawArrays(GL_LINES, 0, 24);
}
