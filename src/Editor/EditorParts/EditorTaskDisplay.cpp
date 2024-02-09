#include "Debug/TaskIndicator.hpp"
#include "Editor/EditorParts/EditorParts.hpp"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"

namespace Shadow::Editor::EditorParts {

void taskDisplayInit() {}

void taskDisplayUpdate() {

	if (TaskIndicator::getActiveTasks().empty()) return;

	ImGuiWindowFlags window_flags
		= ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoFocusOnAppearing
		| ImGuiWindowFlags_NoNav
		| ImGuiWindowFlags_NoMove;

	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowBgAlpha(0.35f);
	ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x - 10.0f, viewport->WorkSize.y - 10.0f), 0, ImVec2(1.0f, 1.0f));
	ImGui::Begin("Engine Task Display", nullptr, window_flags);

	ImGui::Text("Active Tasks");
	ImGui::Separator();

	for (const auto & [key, value] : TaskIndicator::getActiveTasks()) {
		ImGui::Spinner(value.c_str(), 12.0f, 4, IM_COL32(255, 0, 0, 255));
		ImGui::SameLine();
		ImGui::Text("%s", value.c_str());
	}

	ImGui::End();

}

}