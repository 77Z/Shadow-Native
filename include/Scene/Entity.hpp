#ifndef SHADOW_NATIVE_SCENE_ENTITY_HPP
#define SHADOW_NATIVE_SCENE_ENTITY_HPP

#include "Scene/Scene.hpp"
#include <entt.hpp>

namespace Shadow {

class Entity {
public:
	Entity(entt::entity handle, Scene* scene);
	// Entity(const Entity& other) = default;

	template <typename T> bool addComponent() { return scene->m_Registry.emplace<T>(entityHandle); }

private:
	entt::entity entityHandle;
	Scene* scene;
};

}

#endif /* SHADOW_NATIVE_SCENE_ENTITY_HPP */