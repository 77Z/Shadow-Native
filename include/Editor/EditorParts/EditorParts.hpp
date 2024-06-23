#ifndef SHADOW_NATIVE_EDITOR_EDITOR_PARTS_EDITOR_PARTS_HPP
#define SHADOW_NATIVE_EDITOR_EDITOR_PARTS_EDITOR_PARTS_HPP

#include "Editor/Editor.hpp"
namespace Shadow::Editor::EditorParts {

// TODO: Should this be made into a class for public and private accessors?

// CALL THIS
void init(EditorPartsCarePackage carePackage);
void onUpdate();
void destroy();

void showProjectPreferences();
void hideProjectPreferences();

void showEnginePreferences();
void hideEnginePreferences();

void showRawSceneEditor();
void hideRawSceneEditor();

float getViewportWidth();
float getViewportHeight();
bool isMouseOverViewport();

void openCommandCenter();
void closeCommandCenter();
void toggleCommandCenter();

// DON'T CALL THIS
void projectPreferencesInit();
void projectPreferencesUpdate();

void enginePreferencesInit();
void enginePreferencesUpdate();

void rawSceneEditorInit();
void rawSceneEditorUpdate();

void rootWindowInit();
void rootWindowUpdate();
void rootWindowDestroy();

void viewportWindowInit(EditorPartsCarePackage carePackage);
void viewportWindowUpdate();

void resourcesWindowInit();
void resourcesWindowUpdate();

void taskDisplayInit();
void taskDisplayUpdate();

void chunkerWindowUpdate();

void commandCenterInit(EditorPartsCarePackage carePackage);
void commandCenterUpdate();

void sceneOutlineUpdate();

}

#endif /* SHADOW_NATIVE_EDITOR_EDITOR_PARTS_EDITOR_PARTS_HPP */