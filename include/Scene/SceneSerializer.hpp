#ifndef SHADOW_NATIVE_SCENE_SCENE_SERIALIZER_HPP
#define SHADOW_NATIVE_SCENE_SCENE_SERIALIZER_HPP

#include "Core.hpp"
#include "Scene/Scene.hpp"
#include "json_impl.hpp"

namespace Shadow {

class SceneSerializer {
public:
	SceneSerializer(const Reference<Scene>& scene);
	~SceneSerializer();

	void serialize(const std::string filepath);

	/* Returns true on success, false on failure */
	bool deserialize(const std::string filepath);

private:
	Reference<Scene> scene;
	json sceneJson;

	void serializeEntity(Entity entity);
};

}

#endif /* SHADOW_NATIVE_SCENE_SCENE_SERIALIZER_HPP */