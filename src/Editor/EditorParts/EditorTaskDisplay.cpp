#include "Debug/TaskIndicator.hpp"
#include "Editor/EditorParts/EditorParts.hpp"
#include "imgui.h"

namespace Shadow::Editor::EditorParts {

void taskDisplayInit() {}

void taskDisplayUpdate() {

	// * Running engine tasks
	ImGui::Begin("Engine Task Display");

	for (const auto & [key, value] : TaskIndicator::getActiveTasks()) {
		ImGui::Text("%s", value.c_str());
	}

	ImGui::End();

}

}