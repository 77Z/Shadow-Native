#include "Editor/ProjectPreferencesPanel.hpp"
#include "imgui.h"

namespace Shadow {
namespace Editor {

	ProjectPreferencesPanel::ProjectPreferencesPanel() { }
	ProjectPreferencesPanel::~ProjectPreferencesPanel() { }

	void ProjectPreferencesPanel::onUpdate() {
		ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiCond_FirstUseEver);
		if (ImGui::BeginPopupModal("ProjectPreferencesPanel")) {

			ImGui::Text("Bruh");

			ImGui::EndPopup();
		}
	}

	void ProjectPreferencesPanel::open() { ImGui::OpenPopup("ProjectPreferencesPanel"); }

	void ProjectPreferencesPanel::unload() { }

}
}