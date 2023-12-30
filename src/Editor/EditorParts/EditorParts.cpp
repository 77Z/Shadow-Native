#include "Editor/EditorParts/EditorParts.hpp"

namespace Shadow::Editor::EditorParts {

void init() {
	projectPreferencesInit();
	enginePreferencesInit();
}

void onUpdate() {
	projectPreferencesUpdate();
	enginePreferencesUpdate();
}

}