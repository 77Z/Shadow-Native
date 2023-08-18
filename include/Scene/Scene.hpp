#ifndef SHADOW_NATIVE_SCENE_SCENE_HPP
#define SHADOW_NATIVE_SCENE_SCENE_HPP

#include "uuid_impl.hpp"
#include <bgfx/bgfx.h>
#include <entt.hpp>
#include <string>
#include <unordered_map>

namespace Shadow {

// Forward Decl
class Entity;

class Scene {
public:
	Scene();
	Scene(std::string name);
	~Scene();

	// entt::entity createEntity();
	Entity createEntity(const std::string& name = std::string());
	Entity createEntityWithUUID(uuids::uuid uuid, const std::string& name);
	void destroyEntity(Entity entity);

	std::string sceneName = "Untitled Scene";
	entt::registry m_Registry;

	bgfx::VertexLayout pcvDecl;
	bgfx::VertexBufferHandle vbh;
	bgfx::IndexBufferHandle ibh;

	void onUpdate(bgfx::ViewId view, bgfx::ProgramHandle program);
	void unload();

	std::unordered_map<uuids::uuid, entt::entity> entityMap;

private:
	void init();

	friend class Entity;
};

}

#endif /* SHADOW_NATIVE_SCENE_SCENE_HPP */