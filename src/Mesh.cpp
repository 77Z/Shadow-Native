#include "Mesh.hpp"
#include "Debug/Logger.hpp"
#include "Util.hpp"
#include "meshoptimizer/meshoptimizer.h"
#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <bx/readerwriter.h>

#include <tinystl/allocator.h>
#include <tinystl/string.h>
#include <tinystl/vector.h>
namespace stl = tinystl;

namespace Shadow {

Group::Group() { reset(); }

void Group::reset() {
	m_vbh.idx = bgfx::kInvalidHandle;
	m_ibh.idx = bgfx::kInvalidHandle;
	m_numVertices = 0;
	m_vertices = NULL;
	m_numIndices = 0;
	m_indices = NULL;
	m_prims.clear();
}

Mesh::Mesh(const char* _filePath, bool _ramcopy) { load(_filePath, _ramcopy); }

namespace special {
	int32_t read(bx::ReaderI* _reader, bgfx::VertexLayout& _layout, bx::Error* _err);
}

void Mesh::load(const char* _filePath, bool _ramcopy) {
	bx::FileReaderI* reader = getFileReader();

	// TODO: This will probably continue on even if the file can't be loaded
	if (!bx::open(reader, _filePath))
		ERROUT("Failed to open file");

	constexpr uint32_t kChunkVertexBuffer = BX_MAKEFOURCC('V', 'B', ' ', 0x1);
	constexpr uint32_t kChunkVertexBufferCompressed = BX_MAKEFOURCC('V', 'B', 'C', 0x0);
	constexpr uint32_t kChunkIndexBuffer = BX_MAKEFOURCC('I', 'B', ' ', 0x0);
	constexpr uint32_t kChunkIndexBufferCompressed = BX_MAKEFOURCC('I', 'B', 'C', 0x1);
	constexpr uint32_t kChunkPrimitive = BX_MAKEFOURCC('P', 'R', 'I', 0x0);

	using namespace bx;
	using namespace bgfx;

	Group group;

	bx::AllocatorI* allocator = getAllocator();

	uint32_t chunk;
	bx::Error err;
	while (4 == bx::read(reader, chunk, &err) && err.isOk()) {
		switch (chunk) {
		case kChunkVertexBuffer: {
			read(reader, group.m_sphere, &err);
			read(reader, group.m_aabb, &err);
			read(reader, group.m_obb, &err);

			bx::read(reader, m_layout, &err);

			uint16_t stride = m_layout.getStride();

			bx::read(reader, group.m_numVertices, &err);
			const bgfx::Memory* mem = bgfx::alloc(group.m_numVertices * stride);
			bx::read(reader, mem->data, mem->size, &err);

			if (_ramcopy) {
				group.m_vertices = (uint8_t*)bx::alloc(allocator, group.m_numVertices * stride);
				bx::memCopy(group.m_vertices, mem->data, mem->size);
			}

			group.m_vbh = bgfx::createVertexBuffer(mem, m_layout);
		} break;

		case kChunkVertexBufferCompressed: {
			read(reader, group.m_sphere, &err);
			read(reader, group.m_aabb, &err);
			read(reader, group.m_obb, &err);

			read(reader, m_layout, &err);

			uint16_t stride = m_layout.getStride();

			read(reader, group.m_numVertices, &err);

			const bgfx::Memory* mem = bgfx::alloc(group.m_numVertices * stride);

			uint32_t compressedSize;
			bx::read(reader, compressedSize, &err);

			void* compressedVertices = bx::alloc(allocator, compressedSize);
			bx::read(reader, compressedVertices, compressedSize, &err);

			meshopt_decodeVertexBuffer(mem->data, group.m_numVertices, stride,
				(uint8_t*)compressedVertices, compressedSize);

			bx::free(allocator, compressedVertices);

			if (_ramcopy) {
				group.m_vertices = (uint8_t*)bx::alloc(allocator, group.m_numVertices * stride);
				bx::memCopy(group.m_vertices, mem->data, mem->size);
			}

			group.m_vbh = bgfx::createVertexBuffer(mem, m_layout);
		} break;

		case kChunkIndexBuffer: {
			read(reader, group.m_numIndices, &err);

			const bgfx::Memory* mem = bgfx::alloc(group.m_numIndices * 2);
			read(reader, mem->data, mem->size, &err);

			if (_ramcopy) {
				group.m_indices = (uint16_t*)bx::alloc(allocator, group.m_numIndices * 2);
				bx::memCopy(group.m_indices, mem->data, mem->size);
			}

			group.m_ibh = bgfx::createIndexBuffer(mem);
		} break;

		case kChunkIndexBufferCompressed: {
			bx::read(reader, group.m_numIndices, &err);

			const bgfx::Memory* mem = bgfx::alloc(group.m_numIndices * 2);

			uint32_t compressedSize;
			bx::read(reader, compressedSize, &err);

			void* compressedIndices = bx::alloc(allocator, compressedSize);

			bx::read(reader, compressedIndices, compressedSize, &err);

			meshopt_decodeIndexBuffer(
				mem->data, group.m_numIndices, 2, (uint8_t*)compressedIndices, compressedSize);

			bx::free(allocator, compressedIndices);

			if (_ramcopy) {
				group.m_indices = (uint16_t*)bx::alloc(allocator, group.m_numIndices * 2);
				bx::memCopy(group.m_indices, mem->data, mem->size);
			}

			group.m_ibh = bgfx::createIndexBuffer(mem);
		} break;

		case kChunkPrimitive: {
			uint16_t len;
			read(reader, len, &err);

			stl::string material;
			material.resize(len);
			read(reader, const_cast<char*>(material.c_str()), len, &err);

			uint16_t num;
			read(reader, num, &err);

			for (uint32_t ii = 0; ii < num; ++ii) {
				read(reader, len, &err);

				stl::string name;
				name.resize(len);
				read(reader, const_cast<char*>(name.c_str()), len, &err);

				Primitive prim;
				read(reader, prim.m_startIndex, &err);
				read(reader, prim.m_numIndices, &err);
				read(reader, prim.m_startVertex, &err);
				read(reader, prim.m_numVertices, &err);
				read(reader, prim.m_sphere, &err);
				read(reader, prim.m_aabb, &err);
				read(reader, prim.m_obb, &err);

				group.m_prims.push_back(prim);
			}

			m_groups.push_back(group);
			group.reset();
		} break;

		default:
			PRINT("%08x at %d", chunk, bx::skip(reader, 0));
			break;
		}
	}

	bx::close(reader);
}

void Mesh::unload() {
	WARN("Unloading mesh...");
	bx::AllocatorI* allocator = getAllocator();

	for (GroupArray::const_iterator it = m_groups.begin(), itEnd = m_groups.end(); it != itEnd;
		 ++it) {
		const Group& group = *it;
		WARN("VBH is valid, destroying...");
		bgfx::destroy(group.m_vbh);

		if (bgfx::isValid(group.m_ibh)) {
			WARN("IBH is valid, destroying...");
			bgfx::destroy(group.m_ibh);
		}

		if (NULL != group.m_vertices) {
			bx::free(allocator, group.m_vertices);
		}

		if (NULL != group.m_indices) {
			bx::free(allocator, group.m_indices);
		}
	}
	m_groups.clear();
}

void Mesh::submit(
	bgfx::ViewId _id, bgfx::ProgramHandle _program, const float* _mtx, uint64_t _state) const {
	if (BGFX_STATE_MASK == _state) {
		_state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CCW | BGFX_STATE_MSAA;
	}

	bgfx::setTransform(_mtx);
	bgfx::setState(_state);

	for (GroupArray::const_iterator it = m_groups.begin(), itEnd = m_groups.end(); it != itEnd; it++) {
		const Group& group = *it;

		bgfx::setIndexBuffer(group.m_ibh);
		bgfx::setVertexBuffer(0, group.m_vbh);
		bgfx::submit(_id, _program, 0, BGFX_DISCARD_INDEX_BUFFER | BGFX_DISCARD_VERTEX_STREAMS);
	}

	bgfx::discard();
}

void Mesh::submit(const MeshState* const* _state, uint8_t _numPasses, const float* _mtx,
	uint16_t _numMatrices) const {
	uint32_t cached = bgfx::setTransform(_mtx, _numMatrices);

	for (uint32_t pass = 0; pass < _numPasses; ++pass) {
		bgfx::setTransform(cached, _numMatrices);

		const MeshState& state = *_state[pass];
		bgfx::setState(state.m_state);

		for (uint8_t tex = 0; tex < state.m_numTextures; ++tex) {
			const MeshState::Texture& texture = state.m_textures[tex];
			bgfx::setTexture(
				texture.m_stage, texture.m_sampler, texture.m_texture, texture.m_flags);
		}

		for (GroupArray::const_iterator it = m_groups.begin(), itEnd = m_groups.end(); it != itEnd;
			 ++it) {
			const Group& group = *it;

			bgfx::setIndexBuffer(group.m_ibh);
			bgfx::setVertexBuffer(0, group.m_vbh);
			bgfx::submit(state.m_viewId, state.m_program, 0,
				BGFX_DISCARD_INDEX_BUFFER | BGFX_DISCARD_VERTEX_STREAMS);
		}

		bgfx::discard(0 | BGFX_DISCARD_BINDINGS | BGFX_DISCARD_STATE | BGFX_DISCARD_TRANSFORM);
	}

	bgfx::discard();
}

}