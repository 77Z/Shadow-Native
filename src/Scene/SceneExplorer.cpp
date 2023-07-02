#include "Scene/SceneExplorer.hpp"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"
#include "imgui.h"
#include <string>

namespace Shadow {

SceneExplorer::SceneExplorer(Scene& scene)
	: scene(scene) { }

SceneExplorer::~SceneExplorer() { }

void SceneExplorer::onUpdate() {
	ImGui::Begin("Scene Explorer");

	static int selection_mask = (1 << 2);
	int node_clicked = -1;
	int i = 0;
	scene.m_Registry.each([this, &i, &node_clicked](auto entity) {
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			node_clicked = i;

		const bool isSelected = (selection_mask & (1 << i)) != 0;
		if (isSelected)
			nodeFlags |= ImGuiTreeNodeFlags_Selected;

		std::string tag = scene.m_Registry.get<TagComponent>(entity).tag;
		tag.append(" [" + std::to_string(i) + "]");
		ImGui::TreeNodeEx(tag.c_str(), nodeFlags);
		// ImGui::Text("%s", scene.m_Registry.get<TagComponent>(entity).tag.c_str());
		i++;
	});

	if (node_clicked != -1) {
		// Update selection state
		// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
		if (ImGui::GetIO().KeyCtrl)
			selection_mask ^= (1 << node_clicked); // CTRL+click to toggle
		else // if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you
			 // want, may want to preserve selection when clicking on item that is part of the
			 // selection
			selection_mask = (1 << node_clicked); // Click to single-select
	}
	// ImGui::EndTable();

	ImGui::End();
}

}