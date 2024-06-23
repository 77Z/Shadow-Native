#ifndef SHADOW_NATIVE_MESH_HPP
#define SHADOW_NATIVE_MESH_HPP

#include <bgfx/bgfx.h>
#include <bx/bounds.h>
#include <cstdint>

#include <string>
#include <tinystl/allocator.h>
#include <tinystl/vector.h>
namespace stl = tinystl;

namespace Shadow {

struct MeshState {
	struct Texture {
		uint32_t m_flags;
		bgfx::UniformHandle m_sampler;
		bgfx::TextureHandle m_texture;
		uint8_t m_stage;
	};

	Texture m_textures[4];
	uint64_t m_state;
	bgfx::ProgramHandle m_program;
	uint8_t m_numTextures;
	bgfx::ViewId m_viewId;
};

struct Primitive {
	uint32_t m_startIndex;
	uint32_t m_numIndices;
	uint32_t m_startVertex;
	uint32_t m_numVertices;

	bx::Sphere m_sphere;
	bx::Aabb m_aabb;
	bx::Obb m_obb;
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
	bx::Aabb m_aabb;
	bx::Obb m_obb;
	PrimitiveArray m_prims;
};
typedef stl::vector<Group> GroupArray;

class Mesh {
public:
	Mesh() {};
	Mesh(const char* _filePath, bool _ramcopy = false);

	void unload();
	void submit(bgfx::ViewId _id, bgfx::ProgramHandle _program, const float* _mtx,
		uint64_t _state = BGFX_STATE_MASK) const;
	void submit(const MeshState* const* _state, uint8_t _numPasses, const float* _mtx,
		uint16_t _numMatrices = 1) const;

	bgfx::VertexLayout m_layout;

	std::string m_filepath = "";

	void load(const char* _filePath, bool _ramcopy = false);

private:
	GroupArray m_groups;
};

}

#endif /* SHADOW_NATIVE_MESH_HPP */