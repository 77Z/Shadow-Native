#ifndef SHADOW_NATIVE_SCENE_SCENE_EXPLORER_HPP
#define SHADOW_NATIVE_SCENE_SCENE_EXPLORER_HPP

#include "Scene/Scene.hpp"
namespace Shadow {

class SceneExplorer {
public:
	SceneExplorer(Scene& scene);
	~SceneExplorer();

	void onUpdate(entt::entity& entity);

private:
	Scene& scene;
	int selectedNode = -1;
};

}

#endif /* SHADOW_NATIVE_SCENE_SCENE_EXPLORER_HPP */