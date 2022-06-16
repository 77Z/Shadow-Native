//
// Created by Vince on 6/16/22
//

#include <shadow/bgfx-utils.h>

#include <bx/readerwriter.h>

static bx::FileReaderI* fileReader = NULL;
bx::FileReaderI* getFileReader() { return fileReader; }

void Mesh::load(bx::ReaderSeekerI* reader, bool ramcopy)
{
	constexpr uint32_t kChunkVertexBuffer           = BX_MAKEFOURCC('V', 'B', ' ', 0x1);
	constexpr uint32_t kChunkVertexBufferCompressed = BX_MAKEFOURCC('V', 'B', 'C', 0x0);
	constexpr uint32_t kChunkIndexBuffer            = BX_MAKEFOURCC('I', 'B', ' ', 0x0);
	constexpr uint32_t kChunkIndexBufferCompressed  = BX_MAKEFOURCC('I', 'B', 'C', 0x1);
	constexpr uint32_t kChunkPrimitive              = BX_MAKEFOURCC('P', 'R', 'I', 0x0);

	using namespace bx;
	using namespace bgfx;

	Group group;

	bx::AllocatorI* allocator = entry::getAllocator();

	uint32_t chunk;
	bx::Error err;
	while (4 == bx::read(reader, chunk, &err)
	   &&  err.isOk() )
	{
		switch (chunk)
		{
			case kChunkVertexBuffer:
			{
				read(reader, group.m_sphere, &err);
				read(reader, group.m_aabb, &err);
				read(reader, group.m_obb, &err);

				read(reader, m_layout, &err);

				uint16_t stride = m_layout.getStride();

				read(reader, group.m_numVertices, &err);
				const bgfx::Memory* mem = bgfx::alloc(group.m_numVertices*stride);
				read(reader, mem->data, mem->size, &err);

				if (ramcopy)
				{
					group.m_vertices = (uint8_t*)BX_ALLOC(allocator, group.m_numVertices*stride);
					bx::memCopy(group.m_vertices, mem->data, mem->size);
				}

				group.m_vbh = bgfx::createVertexBuffer(mem, m_layout);
			}
				break;

			case kChunkVertexBufferCompressed:
			{
				read(reader, group.m_sphere, &err);
				read(reader, group.m_aabb, &err);
				read(reader, group.m_obb, &err);

				read(reader, m_layout, &err);

				uint16_t stride = m_layout.getStride();

				read(reader, group.m_numVertices, &err);

				const bgfx::Memory* mem = bgfx::alloc(group.m_numVertices*stride);

				uint32_t compressedSize;
				bx::read(reader, compressedSize, &err);

				void* compressedVertices = BX_ALLOC(allocator, compressedSize);
				bx::read(reader, compressedVertices, compressedSize, &err);

				meshopt_decodeVertexBuffer(mem->data, group.m_numVertices, stride, (uint8_t*)compressedVertices, compressedSize);

				BX_FREE(allocator, compressedVertices);

				if (ramcopy)
				{
					group.m_vertices = (uint8_t*)BX_ALLOC(allocator, group.m_numVertices*stride);
					bx::memCopy(group.m_vertices, mem->data, mem->size);
				}

				group.m_vbh = bgfx::createVertexBuffer(mem, m_layout);
			}
				break;

			case kChunkIndexBuffer:
			{
				read(_reader, group.m_numIndices, &err);

				const bgfx::Memory* mem = bgfx::alloc(group.m_numIndices*2);
				read(reader, mem->data, mem->size, &err);

				if (ramcopy)
				{
					group.m_indices = (uint16_t*)BX_ALLOC(allocator, group.m_numIndices*2);
					bx::memCopy(group.m_indices, mem->data, mem->size);
				}

				group.m_ibh = bgfx::createIndexBuffer(mem);
			}
				break;

			case kChunkIndexBufferCompressed:
			{
				bx::read(_reader, group.m_numIndices, &err);

				const bgfx::Memory* mem = bgfx::alloc(group.m_numIndices*2);

				uint32_t compressedSize;
				bx::read(reader, compressedSize, &err);

				void* compressedIndices = BX_ALLOC(allocator, compressedSize);

				bx::read(reader, compressedIndices, compressedSize, &err);

				meshopt_decodeIndexBuffer(mem->data, group.m_numIndices, 2, (uint8_t*)compressedIndices, compressedSize);

				BX_FREE(allocator, compressedIndices);

				if (ramcopy)
				{
					group.m_indices = (uint16_t*)BX_ALLOC(allocator, group.m_numIndices*2);
					bx::memCopy(group.m_indices, mem->data, mem->size);
				}

				group.m_ibh = bgfx::createIndexBuffer(mem);
			}
				break;

			case kChunkPrimitive:
			{
				uint16_t len;
				read(reader, len, &err);

				stl::string material;
				material.resize(len);
				read(_reader, const_cast<char*>(material.c_str() ), len, &err);

				uint16_t num;
				read(reader, num, &err);

				for (uint32_t ii = 0; ii < num; ++ii)
				{
					read(reader, len, &err);

					stl::string name;
					name.resize(len);
					read(_reader, const_cast<char*>(name.c_str() ), len, &err);

					Primitive prim;
					read(_reader, prim.m_startIndex, &err);
					read(_reader, prim.m_numIndices, &err);
					read(_reader, prim.m_startVertex, &err);
					read(_reader, prim.m_numVertices, &err);
					read(_reader, prim.m_sphere, &err);
					read(_reader, prim.m_aabb, &err);
					read(_reader, prim.m_obb, &err);

					group.m_prims.push_back(prim);
				}

				m_groups.push_back(group);
				group.reset();
			}
				break;

			default:
				DBG("%08x at %d", chunk, bx::skip(reader, 0) );
				break;
		}
	}
}

// This pretty much just makes a new object for you
Mesh* meshLoad(bx::ReaderSeekerI* reader, bool ramcopy) {
	Mesh* mesh = new Mesh;
	mesh->load(reader, ramcopy);
	return mesh;
}

// Reads a file from a string and just pushes to the other meshLoad method
Mesh* meshLoad(const char* filepath, bool ramcopy) {
	bx::FileReaderI* reader = getFileReader();
	if (bx::open(reader, filepath)) {
		Mesh* mesh = meshLoad(reader, ramcopy);
		bx::close(reader);
		return mesh;
	}
}
