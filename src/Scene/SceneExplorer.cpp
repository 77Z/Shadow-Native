#include "Scene/SceneExplorer.hpp"
#include "Logger.h"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"
#include "imgui.h"
#include "imgui/imgui_entt_entity_editor.hpp"
#include <string>

namespace MM {
template <>
void ComponentEditorWidget<Shadow::TransformComponent>(
	entt::registry& reg, entt::registry::entity_type e) {
	auto& t = reg.get<Shadow::TransformComponent>(e);
	ImGui::SeparatorText("Translation");
	ImGui::DragFloat("tx", &t.translation.x, 0.05f);
	ImGui::DragFloat("ty", &t.translation.y, 0.05f);
	ImGui::DragFloat("tz", &t.translation.z, 0.05f);

	ImGui::SeparatorText("Rotation");
	ImGui::DragFloat("rx", &t.rotation.x, 0.05f);
	ImGui::DragFloat("ry", &t.rotation.y, 0.05f);
	ImGui::DragFloat("rz", &t.rotation.z, 0.05f);

	ImGui::SeparatorText("Scale");
	ImGui::DragFloat("sx", &t.scale.x, 0.05f);
	ImGui::DragFloat("sy", &t.scale.y, 0.05f);
	ImGui::DragFloat("sz", &t.scale.z, 0.05f);
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