#include "Editor/EditorParts/EditorParts.hpp"
#include "imgui.h"

namespace Shadow::Editor::EditorParts {

void init(EditorPartsCarePackage carePackage) {
	// * Popups
	projectPreferencesInit();
	enginePreferencesInit();

	// * Constant Windows
	rootWindowInit();
	viewportWindowInit(carePackage);
	resourcesWindowInit();
	taskDisplayInit();
}

void onUpdate() {
	// * Popups
	projectPreferencesUpdate();
	enginePreferencesUpdate();

	// * Constant Windows
	rootWindowUpdate();
	ImGui::ShowMetricsWindow();
	ImGui::ShowDemoWindow();
	viewportWindowUpdate();
	resourcesWindowUpdate();
	taskDisplayUpdate();
	chunkerWindowUpdate();
}

void destroy() {
	// * Popups

	// * Constant Windows
	rootWindowDestroy();
}

}