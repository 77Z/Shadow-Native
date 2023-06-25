#ifndef SHADOW_NATIVE_MESH_H
#define SHADOW_NATIVE_MESH_H

#include "bgfx/bgfx.h"
#include <vector>

namespace Shadow {

class Mesh {
public:
	Mesh();
	~Mesh();

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	void load();
	void unload();
	void submit();

	typedef std::vector<Group> GroupArray;

	bgfx::VertexLayout m_layout;
	GroupArr
};

}

#endif