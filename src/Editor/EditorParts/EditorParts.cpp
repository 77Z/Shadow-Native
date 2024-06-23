#include "Editor/EditorParts/EditorParts.hpp"
#include "imgui.h"

namespace Shadow::Editor::EditorParts {

void init(EditorPartsCarePackage carePackage) {
	// * Popups
	projectPreferencesInit();
	enginePreferencesInit();
	rawSceneEditorInit();

	// * Constant Windows
	rootWindowInit();
	viewportWindowInit(carePackage);
	resourcesWindowInit();
	taskDisplayInit();
	commandCenterInit(carePackage);
}

void onUpdate() {
	// * Popups
	projectPreferencesUpdate();
	enginePreferencesUpdate();

	rawSceneEditorUpdate();

	// * Constant Windows
	rootWindowUpdate();
	ImGui::ShowMetricsWindow();
	ImGui::ShowDemoWindow();
	viewportWindowUpdate();
	resourcesWindowUpdate();
	taskDisplayUpdate();
	chunkerWindowUpdate();
	commandCenterUpdate();
	sceneOutlineUpdate();
}

void destroy() {
	// * Popups

	// * Constant Windows
	rootWindowDestroy();
}

}