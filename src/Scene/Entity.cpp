#include "Scene/Entity.hpp"

namespace Shadow {

Entity::Entity(entt::entity handle, Scene* scene)
	: entityHandle(handle)
	, scene(scene) { }

}