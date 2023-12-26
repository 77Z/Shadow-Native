#include "Editor/EditorParts/EditorParts.hpp"

namespace Shadow::Editor::EditorParts {

void init() {
	projectPreferencesInit();
}

void onUpdate() {
	projectPreferencesUpdate();
}

}