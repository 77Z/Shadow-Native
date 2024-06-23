#ifndef SHADOW_NATIVE_SCENE_ENTITY_HPP
#define SHADOW_NATIVE_SCENE_ENTITY_HPP

#include "Debug/Logger.hpp"
#include "Scene/Components.hpp"
#include "Scene/Scene.hpp"
#include "ppk_assert_impl.hpp"
#include "uuid_impl.hpp"
#include <cstdint>
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

	uuids::uuid getUUID() { return GetComponent<IDComponent>().ID; }

	operator bool() const { return entityHandle != entt::null; }
	operator entt::entity() const { return entityHandle; }
	operator uint32_t() const { return (uint32_t)entityHandle; }

private:
	entt::entity entityHandle { entt::null };
	Scene* scene = nullptr;
};

}

#endif /* SHADOW_NATIVE_SCENE_ENTITY_HPP */