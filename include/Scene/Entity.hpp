#ifndef SHADOW_NATIVE_SCENE_ENTITY_HPP
#define SHADOW_NATIVE_SCENE_ENTITY_HPP

#include "Scene/Scene.hpp"
#include "ppk_assert.h"
#include <entt.hpp>
#include <utility>

namespace Shadow {

class Entity {
public:
	Entity() = default;
	Entity(entt::entity handle, Scene* scene);
	Entity(const Entity& other) = default;

	template <typename T, typename... Args> T& addComponent(Args&&... args) {
		PPK_ASSERT(!hasComponent<T>(), "Entity already has component");
		return scene->m_Registry.emplace<T>(entityHandle, std::forward<Args>(args)...);
	}

	template <typename T> T& GetComponent() {
		PPK_ASSERT(hasComponent<T>(), "Entity has no such component");
		return scene->m_Registry.get<T>(entityHandle);
	}

	template <typename T> bool hasComponent() { return scene->m_Registry.all_of<T>(entityHandle); }

	template <typename T> void removeComponent() {
		PPK_ASSERT(hasComponent<T>(), "Entity has no such component");
		scene->m_Registry.remove<T>(entityHandle);
	}

	// operator bool() const { return entityHandle != 0; }

private:
	entt::entity entityHandle { 0 };
	Scene* scene = nullptr;
};

}

#endif /* SHADOW_NATIVE_SCENE_ENTITY_HPP */