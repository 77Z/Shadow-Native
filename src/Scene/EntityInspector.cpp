#include "Scene/EntityInspector.hpp"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "imgui.h"
#include <string>

namespace Shadow {

	EntityInspector::EntityInspector() {}
	EntityInspector::~EntityInspector() {}

	void EntityInspector::onUpdate() {
		
		ImGui::Begin("Entity Inspector");

		if (entity == nullptr) {
			ImGui::Text("Select an Entity to show data");
		} else {
			ImGui::Text("%s", entity->GetComponent<TagComponent>().tag.c_str());
		}

		ImGui::End();

	}

	void EntityInspector::unload() {}

}