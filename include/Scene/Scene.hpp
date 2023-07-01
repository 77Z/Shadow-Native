#ifndef SHADOW_NATIVE_SCENE_SCENE_HPP
#define SHADOW_NATIVE_SCENE_SCENE_HPP

#include <bgfx/bgfx.h>
#include <entt.hpp>
#include <string>

namespace Shadow {

// Forward Decl
class Entity;

class Scene {
public:
	Scene();
	~Scene();

	// entt::entity createEntity();
	Entity createEntity(const std::string& name = std::string());
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