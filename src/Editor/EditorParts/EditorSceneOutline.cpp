#include "Core.hpp"
#include "Debug/Logger.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorParts/EditorParts.hpp"
#include "Scene/Components.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"
#include "imgui.h"
#include "uuid_impl.hpp"
#include <string>

namespace Shadow::Editor::EditorParts {

static uuids::uuid selectedEntityUUID;

void sceneOutlineUpdate() {
	ImGui::Begin("Scene Outline");

	Reference<Scene> scene = getCurrentScene();

	// Every Entity has a tag
	for (auto entityShell : scene->m_Registry.view<TagComponent>()) {
		Entity entity = {entityShell, scene.get()};

		std::string& entityTag = entity.GetComponent<TagComponent>().tag;

		if (ImGui::Selectable(entityTag.c_str())) {
			selectedEntityUUID = entity.getUUID();
		}
	}

	ImGui::SeparatorText("inspector");

	if (selectedEntityUUID.is_nil()) {
		ImGui::End();
		return;
	}

	ImGui::Text("%s", uuids::to_string(selectedEntityUUID).c_str());

	for (auto entityShell : scene->m_Registry.view<TagComponent>()) {
		Entity entity = {entityShell, scene.get()};

		if (selectedEntityUUID != entity.getUUID())
			continue;


		std::string& entityTag = entity.GetComponent<TagComponent>().tag;

		ImGui::Text("%s", entityTag.c_str());

		if (entity.hasComponent<TransformComponent>()) {
			auto& component = entity.GetComponent<TransformComponent>();

			ImGui::SeparatorText("Transform");
			ImGui::DragFloat("trx", &component.translation.x);
			ImGui::DragFloat("try", &component.translation.y);
			ImGui::DragFloat("trz", &component.translation.z);
			ImGui::DragFloat("rox", &component.rotation.x);
			ImGui::DragFloat("roy", &component.rotation.y);
			ImGui::DragFloat("roz", &component.rotation.z);
			ImGui::DragFloat("scx", &component.scale.x);
			ImGui::DragFloat("scy", &component.scale.y);
			ImGui::DragFloat("scz", &component.scale.z);
		}

		if (entity.hasComponent<MeshComponent>()) {
			auto& component = entity.GetComponent<MeshComponent>();

			ImGui::SeparatorText("Mesh");
			ImGui::Text("File path: %s", component.mesh.m_filepath.c_str());
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drop .mesh files here to swap");
			if (ImGui::BeginDragDropTarget()) {
				ImGuiDragDropFlags drop_target_flags = ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoPreviewTooltip;
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, drop_target_flags))
				{
					if (payload->IsDataType("ShadowMeshFile")) {
						if (payload->IsDelivery()) {
							WARN("PAYLOAD DELIVERED");
						}
					} else {
						ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
						ImGui::BeginTooltip();
						ImGui::Text("Mesh files only!");
						ImGui::EndTooltip();
					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::Separator();
		ImGui::Button("Drag me");
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetTooltip("SHADOW MESH");
			ImGui::SetDragDropPayload("ShadowMeshFile", "Bruh", sizeof("Bruh"));
			ImGui::EndDragDropSource();
		}

		break;
	}

	// std::string& entityTag = selectedEntity->GetComponent<TagComponent>().tag;
	// ImGui::Text("󰆧 %s", entityTag.c_str());

	ImGui::End();
}


}