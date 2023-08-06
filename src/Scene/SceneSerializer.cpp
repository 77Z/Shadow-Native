#include "Scene/SceneSerializer.hpp"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"
#include "Util.h"

namespace Shadow {

static void serializeEntity(Entity entity) {
	PPK_ASSERT(entity.hasComponent<TagComponent>());

	if (entity.hasComponent<TransformComponent>()) { }
}

SceneSerializer::SceneSerializer(const Reference<Scene>& scene)
	: scene(scene) { }

SceneSerializer::~SceneSerializer() { }

}