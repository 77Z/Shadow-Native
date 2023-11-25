#include "DevEntryPoint.hpp"
#include "Core.hpp"
#include "Debug/Logger.hpp"
#include "RenderBootstrapper.hpp"
#include "ShadowWindow.hpp"
#include "UI/ShadowFlinger.hpp"
#include "Util.hpp"
#include "bgfx/defines.h"
#include "bx/allocator.h"
#include "bx/bounds.h"
#include "bx/bx.h"
// #include "Scene/Components.hpp"
// #include "Scene/Entity.hpp"
// #include "Scene/Scene.hpp"
// #include "Scene/SceneSerializer.hpp"
#include "bgfx/bgfx.h"
#include "bx/error.h"
#include "bx/readerwriter.h"
#include "imgui.h"
#include "meshoptimizer/meshoptimizer.h"
#include "snappy.h"
// #include "uuid.h"
// #include "uuid_impl.hpp"

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <tinystl/allocator.h>
#include <tinystl/vector.h>
#include <unistd.h>
namespace stl = tinystl;


namespace Shadow {

static void pad() { PRINT("\n\n\n\n"); }

struct Primitive {
	uint32_t m_startIndex;
	uint32_t m_numIndices;
	uint32_t m_startVertex;
	uint32_t m_numVertices;

	bx::Sphere m_sphere;
	bx::Aabb   m_aabb;
	bx::Obb    m_obb;
};
typedef stl::vector<Primitive> PrimitiveArray;

struct Group {
	Group();
	void reset();

	bgfx::VertexBufferHandle m_vbh;
	bgfx::IndexBufferHandle m_ibh;
	uint16_t m_numVertices;
	uint8_t* m_vertices;
	uint32_t m_numIndices;
	uint16_t* m_indices;
	bx::Sphere m_sphere;
	bx::Aabb   m_aabb;
	bx::Obb    m_obb;
	PrimitiveArray m_prims;
};
typedef stl::vector<Group> GroupArray;


int devEntry() {

	std::ifstream file("./missingtex.ktx");

	std::stringstream ss;
	ss << file.rdbuf();

	std::string in = ss.str();
	std::string out;
	snappy::Compress(in.data(), in.size(), &out);
	std::ofstream outfile("./missingtex.snap.ktx");
	outfile << out;

	/*
	// RenderBootstrapper inits bgfx
	ShadowWindow devWindow(1300, 700, "GURU MEDITATION");
	RenderBootstrapper rb(&devWindow, bgfx::RendererType::Vulkan);
	UI::ShadowFlingerViewport shadowFlinger(12);


	// bgfx::Init init;
	// init.type = bgfx::RendererType::Noop;
	// bgfx::init(init);

	GroupArray groups;
	bgfx::VertexLayout layout;

	pad();

	WARN("Development entry point");

	constexpr uint32_t kChunkVertexBuffer           = BX_MAKEFOURCC('V', 'B', ' ', 0x1);
	constexpr uint32_t kChunkVertexBufferCompressed = BX_MAKEFOURCC('V', 'B', 'C', 0x0);
	constexpr uint32_t kChunkIndexBuffer            = BX_MAKEFOURCC('I', 'B', ' ', 0x0);
	constexpr uint32_t kChunkIndexBufferCompressed  = BX_MAKEFOURCC('I', 'B', 'C', 0x1);
	constexpr uint32_t kChunkPrimitive              = BX_MAKEFOURCC('P', 'R', 'I', 0x0);

	PRINT("kChunkVertexBuffer: %i", kChunkVertexBuffer);
	PRINT("kChunkVertexBufferCompressed: %i", kChunkVertexBufferCompressed);
	PRINT("kChunkIndexBuffer: %i", kChunkIndexBuffer);
	PRINT("kChunkIndexBufferCompressed: %i", kChunkIndexBufferCompressed);
	PRINT("kChunkPrimitive: %i", kChunkPrimitive);

	Group group;
	
	bx::AllocatorI* allocator = getAllocator();

	uint32_t chunk;
	bx::Error err;

	// bx::ReaderSeekerI* reader = getFileReader();
	bx::FileReaderI* reader = getFileReader();
	if (!bx::open(reader, "./cube.bin")) {
		ERROUT("Failed to open file");
		return 1;
	}

	while (bx::read(reader, chunk, &err) == 4 && err.isOk()) {
		switch (chunk) {
			case kChunkVertexBuffer: {
				PRINT("READ VERTEX BUFFER!");

				read(reader, group.m_sphere, &err);
				read(reader, group.m_aabb, &err);
				read(reader, group.m_obb, &err);

				read(reader, layout, &err);

				uint16_t stride = layout.getStride();

				read(reader, group.m_numVertices, &err);
				PRINT("NUMBER OF VERTS IN THIS VERTEX BUFFER: %i", group.m_numVertices);
				const bgfx::Memory* mem = bgfx::alloc(group.m_numVertices * stride);
				read(reader, mem->data, mem->size, &err);

				//TODO: RAMCOPY GOES HERE, FALSE BY DEFAULT, IS IT NEEDED???

				group.m_vbh = bgfx::createVertexBuffer(mem, layout);

				break;
			}
			case kChunkVertexBufferCompressed: {
				PRINT("READ VERTEX BUFFER COMPRESSED");

				read(reader, group.m_sphere, &err);
				read(reader, group.m_aabb, &err);
				read(reader, group.m_obb, &err);

				read(reader, layout, &err);

				uint16_t stride = layout.getStride();

				read(reader, group.m_numVertices, &err);
				const bgfx::Memory* mem = bgfx::alloc(group.m_numVertices * stride);

				uint32_t compressedSize;
				bx::read(reader, compressedSize, &err);

				void* compressedVertices = bx::alloc(allocator, compressedSize);
				bx::read(reader, compressedVertices, compressedSize, &err);

				meshopt_decodeVertexBuffer(mem->data, group.m_numVertices, stride, (uint8_t*)compressedVertices, compressedSize);

				bx::free(allocator, compressedVertices);
					
				//TODO: RAMCOPY GOES HERE, FALSE BY DEFAULT, IS IT NEEDED???
				
				group.m_vbh = bgfx::createVertexBuffer(mem, layout);

				break;
			}
			case kChunkIndexBuffer: {
				PRINT("READ INDEX BUFFER");

				read(reader, group.m_numIndices, &err);

				const bgfx::Memory* mem = bgfx::alloc(group.m_numIndices * 2);
				read(reader, mem->data, mem->size, &err);

				//TODO: RAMCOPY GOES HERE, FALSE BY DEFAULT, IS IT NEEDED???

				group.m_ibh = bgfx::createIndexBuffer(mem);

				break;
			}
			case kChunkIndexBufferCompressed: {
				PRINT("READ INDEX BUFFER COMPRESSED");
						
				read(reader, group.m_numIndices, &err);

				const bgfx::Memory* mem = bgfx::alloc(group.m_numIndices * 2);

				uint32_t compressedSize;
				bx::read(reader, group.m_numIndices, &err);

				void* compressedIndices = bx::alloc(allocator, compressedSize);

				bx::read(reader, compressedIndices, compressedSize, &err);

				meshopt_decodeIndexBuffer(mem->data, group.m_numIndices, 2, (uint8_t*)compressedIndices, compressedSize);

				bx::free(allocator, compressedIndices);

				//TODO: RAMCOPY GOES HERE, FALSE BY DEFAULT, IS IT NEEDED???

				group.m_ibh = bgfx::createIndexBuffer(mem);

				break;
			}
			case kChunkPrimitive: {
				PRINT("READ CHUNK PRIMATIVE");

				uint16_t len;
				read(reader, len, &err);

				std::string material;
				material.resize(len);
				read(reader, const_cast<char*>(material.c_str()), len, &err);

				uint16_t num;
				read(reader, num, &err);

				for (uint32_t ii = 0; ii < num; ii++) {
					read(reader, len, &err);

					std::string name;
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

				groups.push_back(group);
				group.reset();

				break;
			}

			default: {
				PRINT("[MESH DECODE] %08x at %d", chunk, bx::skip(reader, 0));
				break;
			}
		}
	}


#if 0
	// PRINT("Here's a UUID: %s", uuids::to_string(generateUUID()).c_str());

	Reference<Scene> scene = CreateReference<Scene>("My Scene");
	SceneSerializer ss(scene);

	Entity bruh = scene->createEntity("bruh");
	bruh.GetComponent<TagComponent>().tag = "overidden";
	bruh.addComponent<TransformComponent>();
	bruh.GetComponent<TransformComponent>().translation.x = 69.420;

	Entity cube = scene->createEntity("My Cube");
	cube.addComponent<CubeComponent>(1.0f);

	ss.serialize("./scene.sescene");

	WARN("SPLIT");

	Reference<Scene> newScene = CreateReference<Scene>("Doesn't Matter");
	SceneSerializer newSs(newScene);

	newSs.deserialize("./scene.sescene");
#endif

	pad();

	

	// LOAD DATA
	bgfx::ProgramHandle program = loadProgram("test/vs_test.sc.spv", "test/fs_test.sc.spv");

	while (!devWindow.shouldClose()) {
		devWindow.pollEvents();

		rb.startFrame();

		if (devWindow.wasWindowResized()) { shadowFlinger.resized(); }

		ImGui::Begin("Hello");
			
		if (ImGui::Button("Lock Mouse Cursor")) {
			devWindow.lockCursor();
		}

		if (ImGui::Button("Unlock Mouse Cursor")) {
			devWindow.unlockCursor();
		}

		ImGui::End();

		// bgfx::setTransform()
		bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CCW | BGFX_STATE_MSAA);
		
		for (GroupArray::const_iterator it = groups.begin(), itEnd = groups.end(); it != itEnd; it++) {
			const Group& group = *it;

			bgfx::setIndexBuffer(group.m_ibh);
			bgfx::setVertexBuffer(0, group.m_vbh);
			bgfx::submit(1, program, 0, BGFX_DISCARD_INDEX_BUFFER | BGFX_DISCARD_VERTEX_STREAMS);
		}

		bgfx::discard();

		// shadowFlinger.draw(program, devWindow.getExtent().width, devWindow.getExtent().height);

		rb.endFrame();
	}

	shadowFlinger.unload();
	bgfx::destroy(program);

	rb.shutdown();
*/
	return 0;
}

}