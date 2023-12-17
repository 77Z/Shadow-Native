#include "imgui.h"

namespace Shadow::Editor::EditorParts {

static bool shouldDrawProjectPreferences = false;

void showProjectPreferences() { shouldDrawProjectPreferences = true; }
void hideProjectPreferences() { shouldDrawProjectPreferences = false; }

void projectPreferencesUpdate() {

	// TODO: FIX THIS, MAKE THIS THE IMGUI WAY, THIS IS DUMB
	if (!shouldDrawProjectPreferences) return;

	ImGui::Begin("Project Preferences", &shouldDrawProjectPreferences);
	
	ImGui::Text("WHhhhhaatt");

	ImGui::End();

}

}