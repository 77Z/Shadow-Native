#ifndef SHADOW_NATIVE_SCENE_SCENE_HPP
#define SHADOW_NATIVE_SCENE_SCENE_HPP

#include <bgfx/bgfx.h>
#include <entt.hpp>

namespace Shadow {

class Scene {
public:
	Scene();
	~Scene();

	entt::entity createEntity();
	entt::registry m_Registry;

	bgfx::VertexLayout pcvDecl;
	bgfx::VertexBufferHandle vbh;
	bgfx::IndexBufferHandle ibh;

	void onUpdate(bgfx::ProgramHandle program);

private:
	friend class Entity;
};

}

#endif /* SHADOW_NATIVE_SCENE_SCENE_HPP */