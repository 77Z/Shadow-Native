#include "DevEntryPoint.hpp"
#include "Core.hpp"
#include "Debug/Logger.hpp"
#include "Util.hpp"
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
// #include "uuid.h"
// #include "uuid_impl.hpp"

#include <cstdint>
#include <tinystl/allocator.h>
#include <tinystl/vector.h>
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

	bgfx::Init init;
	init.type = bgfx::RendererType::Noop;
	bgfx::init(init);

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
				break;
			}
			case kChunkVertexBufferCompressed: {
				PRINT("READ VERTEX BUFFER COMPRESSED");
				break;
			}
			case kChunkIndexBuffer: {
				PRINT("READ INDEX BUFFER");
				break;
			}
			case kChunkIndexBufferCompressed: {
				PRINT("READ INDEX BUFFER COMPRESSED");
				break;
			}
			case kChunkPrimitive: {
				PRINT("READ CHUNK PRIMATIVE");
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
	return 0;
}

}