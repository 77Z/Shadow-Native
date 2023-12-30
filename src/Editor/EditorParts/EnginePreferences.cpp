#include "Configuration/EngineConfiguration.hpp"
#include "Editor/EditorParts/EditorParts.hpp"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "json_impl.hpp"
#include <string>

namespace Shadow::Editor::EditorParts {

static bool shouldDrawEnginePreferences = false;

static bool settingsDirty = false;

static json engineConfigSec;

struct EnginePreferencesGrouping {
	bool vscodeDefaultEditor;
	std::string alternativeEditor;
};

static EnginePreferencesGrouping enginePreferencesGrouping;

static int preferencesItemEditCallback(ImGuiInputTextCallbackData* data) {
	if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
		settingsDirty = true;
	}
	return 0;
}

void showEnginePreferences() { shouldDrawEnginePreferences = true; }
void hideEnginePreferences() { shouldDrawEnginePreferences = false; }

void enginePreferencesInit() {
	engineConfigSec = JSON::readBsonFile(EngineConfiguration::getConfigDir() + "/engineConfig.sec");

	enginePreferencesGrouping.vscodeDefaultEditor = (bool)engineConfigSec["vscodeDefaultEditor"];
	enginePreferencesGrouping.alternativeEditor = (std::string)engineConfigSec["alternativeEditor"];
}

void enginePreferencesUpdate() {

	if (!shouldDrawEnginePreferences) return;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;

	if (settingsDirty) windowFlags |= ImGuiWindowFlags_UnsavedDocument;

	ImGui::Begin("Engine Preferences", &shouldDrawEnginePreferences, windowFlags);

	ImGui::Checkbox("VSCode as default editor", &enginePreferencesGrouping.vscodeDefaultEditor);
	
	ImGui::BeginDisabled(enginePreferencesGrouping.vscodeDefaultEditor);
	ImGui::InputText("Alternative Editor Path", &enginePreferencesGrouping.alternativeEditor, 0, preferencesItemEditCallback);
	ImGui::EndDisabled();

	ImGui::End();
}

}