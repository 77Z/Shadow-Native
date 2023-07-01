#include "Scene/SceneExplorer.hpp"
#include "Scene/Scene.hpp"
#include "imgui.h"

namespace Shadow {

SceneExplorer::SceneExplorer(Scene* scene)
	: scene(scene) { }
SceneExplorer::~SceneExplorer() { }

void SceneExplorer::onUpdate() {
	ImGui::Begin("Scene Explorer");

	ImGui::End();
}

}