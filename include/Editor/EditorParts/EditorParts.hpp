#ifndef SHADOW_NATIVE_EDITOR_EDITOR_PARTS_EDITOR_PARTS_HPP
#define SHADOW_NATIVE_EDITOR_EDITOR_PARTS_EDITOR_PARTS_HPP

namespace Shadow::Editor::EditorParts {

// CALL THIS
void onUpdate();
void showProjectPreferences();
void hideProjectPreferences();

// DON'T CALL THIS
void projectPreferencesUpdate();

}

#endif /* SHADOW_NATIVE_EDITOR_EDITOR_PARTS_EDITOR_PARTS_HPP */