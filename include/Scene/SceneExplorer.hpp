#ifndef SHADOW_NATIVE_SCENE_SCENE_EXPLORER_HPP
#define SHADOW_NATIVE_SCENE_SCENE_EXPLORER_HPP

#include "Scene/EntityInspector.hpp"
#include "Scene/Scene.hpp"
namespace Shadow {

class SceneExplorer {
public:
	SceneExplorer(Scene& scene, EntityInspector& entityInspector);

	~SceneExplorer();

	void onUpdate();

private:
	Scene& scene;
	EntityInspector& entityInspector;
	int selectedNode = -1;
	
	entt::entity* selectedEntityRef = nullptr;
};

}

#endif /* SHADOW_NATIVE_SCENE_SCENE_EXPLORER_HPP */