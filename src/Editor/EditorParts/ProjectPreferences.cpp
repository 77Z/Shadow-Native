#include "Editor/Project.hpp"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "json_impl.hpp"

namespace Shadow::Editor::EditorParts {

static bool shouldDrawProjectPreferences = false;

static bool settingsDirty = false;

static json projectSec;

struct ProjectPreferencesGrouping {
	std::string name;
	std::string pkgId;
	std::string engineVer;
	std::string defaultScene;
};

static ProjectPreferencesGrouping project;

static int preferencesItemEditCallback(ImGuiInputTextCallbackData* data) {
	if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
		settingsDirty = true;
	}
	return 0;
}

void showProjectPreferences() { shouldDrawProjectPreferences = true; }
void hideProjectPreferences() { shouldDrawProjectPreferences = false; }

void projectPreferencesInit() {
	projectSec = JSON::readBsonFile(Editor::getCurrentProjectPath() + "/project.sec");

	project.name = (std::string)projectSec["name"];
	project.pkgId = (std::string)projectSec["pkgId"];
	project.engineVer = (std::string)projectSec["engineVer"];
	project.defaultScene = (std::string)projectSec["default-scene"];
}

void projectPreferencesUpdate() {

	// TODO: FIX THIS, MAKE THIS THE IMGUI WAY, THIS IS DUMB
	if (!shouldDrawProjectPreferences) return;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;

	if (settingsDirty) windowFlags |= ImGuiWindowFlags_UnsavedDocument;

	ImGui::Begin("Project Preferences", &shouldDrawProjectPreferences, windowFlags);
	
	ImGui::Text("%s", Editor::getCurrentProjectPath().c_str());

	ImGui::InputText("Project Name", &project.name, ImGuiInputTextFlags_CallbackEdit, preferencesItemEditCallback);
	ImGui::InputText("Package ID", &project.pkgId, ImGuiInputTextFlags_CallbackEdit, preferencesItemEditCallback);
	ImGui::InputText("Build for Shadow Engine version", &project.engineVer, ImGuiInputTextFlags_CallbackEdit, preferencesItemEditCallback);
	ImGui::InputText("Boot to scene: ", &project.defaultScene, ImGuiInputTextFlags_CallbackEdit, preferencesItemEditCallback);

	if (ImGui::Button("Save Preferences")) {
		// TODO: Have imgui inputs directly modify json object then serialize it on save???
		settingsDirty = false;
	}

	ImGui::End();

}

}