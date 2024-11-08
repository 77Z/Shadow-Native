#include "Scene/SceneExplorer.hpp"
#include "Debug/Logger.hpp"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "Scene/EntityInspector.hpp"
#include "Scene/Scene.hpp"
#include "imgui.h"
#include "imgui/imgui_entt_entity_editor.hpp"
#include "imgui/imgui_utils.hpp"
#include <string>

namespace MM {
template <>
void ComponentEditorWidget<Shadow::TransformComponent>(
	entt::registry& reg, entt::registry::entity_type e) {
	auto& t = reg.get<Shadow::TransformComponent>(e);
	ImGui::SeparatorText("Translation");
	ImGui::PushItemWidth(80);
	ImGui::DragFloat("x##t", &t.translation.x, 0.05f);
	ImGui::SameLine();
	ImGui::DragFloat("y##t", &t.translation.y, 0.05f);
	ImGui::SameLine();
	ImGui::DragFloat("z##t", &t.translation.z, 0.05f);

	ImGui::SeparatorText("Rotation");
	ImGui::DragFloat("x##r", &t.rotation.x, 0.05f);
	ImGui::SameLine();
	ImGui::DragFloat("y##r", &t.rotation.y, 0.05f);
	ImGui::SameLine();
	ImGui::DragFloat("z##r", &t.rotation.z, 0.05f);

	ImGui::SeparatorText("Scale");
	ImGui::DragFloat("x##s", &t.scale.x, 0.05f);
	ImGui::SameLine();
	ImGui::DragFloat("y##s", &t.scale.y, 0.05f);
	ImGui::SameLine();
	ImGui::DragFloat("z##s", &t.scale.z, 0.05f);
	ImGui::PopItemWidth();
}

template <>
void ComponentEditorWidget<Shadow::TagComponent>(
	entt::registry& reg, entt::registry::entity_type e) {
	auto& t = reg.get<Shadow::TagComponent>(e);
	ImGui::InputText("Tag", &t.tag);
}
}

namespace Shadow {

SceneExplorer::SceneExplorer(Scene& scene, EntityInspector& entityInspector)
	: scene(scene)
	, entityInspector(entityInspector) { }

SceneExplorer::~SceneExplorer() { }

void SceneExplorer::onUpdate() {
	ImGui::Begin("Scene Explorer");
	int i = 0;

	ImGui::Text("selectedNode = %i", selectedNode);

	scene.m_Registry.each([this, &i](auto entity) {
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if (selectedNode == i)
			nodeFlags |= ImGuiTreeNodeFlags_Selected;

		std::string tag = scene.m_Registry.get<TagComponent>(entity).tag;
		tag.append(" [" + std::to_string(i) + "]");

		// ImGui::TreeNodeEx(std::to_string(i).c_str(), nodeFlags);
		ImGui::TreeNodeEx(tag.c_str(), nodeFlags);

		if (ImGui::IsItemClicked()) {
			selectedNode = i;
			
			selectedEntityRef = &entity;
			// entityInspector.pickEntity({entity, scene});
		}
		i++;
	});

	ImGui::End();

	MM::EntityEditor<entt::entity> editor;

	editor.registerComponent<TransformComponent>("Transform");
	editor.registerComponent<TagComponent>("Tag");

	// editor.renderEntityList(scene.m_Registry, std::set<ComponentTypeID> &comp_list)

// #if 0
	ImGui::SetNextWindowSize(ImVec2(550, 400), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Entity Editor")) {
		if (selectedEntityRef != nullptr) {
			if (ImGui::BeginChild("list", { 300, 0 }, true)) {
				static std::set<entt::id_type> comp_list;
				editor.renderEntityList(scene.m_Registry, comp_list);
			}
			ImGui::EndChild();

			ImGui::SameLine();

			if (ImGui::BeginChild("editor")) {
				editor.renderEditor(scene.m_Registry, *selectedEntityRef);
			}
			ImGui::EndChild();
		} else {
			ImGui::Text("No Entity selected.");
		}
	}
	ImGui::End();
// #endif
}

}