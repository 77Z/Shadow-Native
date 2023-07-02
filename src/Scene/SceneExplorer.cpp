#include "Scene/SceneExplorer.hpp"
#include "Logger.h"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"
#include "imgui.h"
#include "imgui/imgui_entt_entity_editor.hpp"
#include "imgui/imgui_utils.h"
#include <string>

namespace MM {
template <>
void ComponentEditorWidget<Shadow::TransformComponent>(
	entt::registry& reg, entt::registry::entity_type e) {
	auto& t = reg.get<Shadow::TransformComponent>(e);
	ImGui::SeparatorText("Translation");
	ImGui::SetNextItemWidth(100.0f);
	ImGui::DragFloat("x##t", &t.translation.x, 0.05f);
	ImGui::DragFloat("y##t", &t.translation.y, 0.05f);
	ImGui::DragFloat("z##t", &t.translation.z, 0.05f);

	ImGui::SeparatorText("Rotation");
	ImGui::DragFloat("x##r", &t.rotation.x, 0.05f);
	ImGui::DragFloat("y##r", &t.rotation.y, 0.05f);
	ImGui::DragFloat("z##r", &t.rotation.z, 0.05f);

	ImGui::SeparatorText("Scale");
	ImGui::DragFloat("x##s", &t.scale.x, 0.05f);
	ImGui::DragFloat("y##s", &t.scale.y, 0.05f);
	ImGui::DragFloat("z##s", &t.scale.z, 0.05f);
}

template <>
void ComponentEditorWidget<Shadow::TagComponent>(
	entt::registry& reg, entt::registry::entity_type e) {
	auto& t = reg.get<Shadow::TagComponent>(e);
	ImGui::InputText("Tag", &t.tag);
}
}

namespace Shadow {

SceneExplorer::SceneExplorer(Scene& scene)
	: scene(scene) { }

SceneExplorer::~SceneExplorer() { }

void SceneExplorer::onUpdate(entt::entity& entity) {
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

		if (ImGui::IsItemClicked())
			selectedNode = i;
		i++;
	});

	ImGui::End();

	MM::EntityEditor<entt::entity> editor;

	editor.registerComponent<TransformComponent>("Transform");
	editor.registerComponent<TagComponent>("Tag");

	// editor.renderEntityList(scene.m_Registry, std::set<ComponentTypeID> &comp_list)

	ImGui::SetNextWindowSize(ImVec2(550, 400), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Entity Editor")) {
		if (ImGui::BeginChild("list", { 300, 0 }, true)) {
			static std::set<entt::id_type> comp_list;
			editor.renderEntityList(scene.m_Registry, comp_list);
		}
		ImGui::EndChild();

		ImGui::SameLine();

		if (ImGui::BeginChild("editor")) {
			editor.renderEditor(scene.m_Registry, entity);
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

}