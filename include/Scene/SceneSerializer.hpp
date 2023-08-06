#ifndef SHADOW_NATIVE_SCENE_SCENE_SERIALIZER_HPP
#define SHADOW_NATIVE_SCENE_SCENE_SERIALIZER_HPP

#include "Scene/Scene.hpp"
#include "Util.h"

namespace Shadow {

class SceneSerializer {
public:
	SceneSerializer(const Reference<Scene>& scene);
	~SceneSerializer();

private:
	Reference<Scene> scene;
};

}

#endif /* SHADOW_NATIVE_SCENE_SCENE_SERIALIZER_HPP */