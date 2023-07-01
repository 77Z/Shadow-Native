#ifndef SHADOW_NATIVE_SCENE_SCENE_EXPLORER_HPP
#define SHADOW_NATIVE_SCENE_SCENE_EXPLORER_HPP

#include "Scene/Scene.hpp"
namespace Shadow {

class SceneExplorer {
public:
	SceneExplorer(Scene* scene);
	~SceneExplorer();

	void onUpdate();

private:
	Scene& scene;
};

}

#endif /* SHADOW_NATIVE_SCENE_SCENE_EXPLORER_HPP */