#include "Editor/Project.hpp"
#include "imgui.h"
#include "json_impl.hpp"

namespace Shadow::Editor::EditorParts {

static bool shouldDrawProjectPreferences = false;

void showProjectPreferences() { shouldDrawProjectPreferences = true; }
void hideProjectPreferences() { shouldDrawProjectPreferences = false; }

void projectPreferencesUpdate() {

	// TODO: FIX THIS, MAKE THIS THE IMGUI WAY, THIS IS DUMB
	if (!shouldDrawProjectPreferences) return;

	ImGui::Begin("Project Preferences", &shouldDrawProjectPreferences);
	
	ImGui::Text("%s", Editor::getCurrentProjectPath().c_str());

	json projectSec = JSON::readBsonFile(Editor::getCurrentProjectPath() + "/project.sec");

	ImGui::Text("Package ID: %s", ((std::string)projectSec["pkgId"]).c_str());
	ImGui::Text("Package ID: %s", ((std::string)projectSec["pkgId"]).c_str());
	ImGui::Text("Build for Shadow Engine version: %s", ((std::string)projectSec["engineVer"]).c_str());

	ImGui::End();

}

}