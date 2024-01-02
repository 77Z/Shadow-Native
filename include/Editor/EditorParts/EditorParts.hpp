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

float getViewportWidth();
float getViewportHeight();
bool isMouseOverViewport();

// DON'T CALL THIS
void projectPreferencesInit();
void projectPreferencesUpdate();

void enginePreferencesInit();
void enginePreferencesUpdate();

void rootWindowInit();
void rootWindowUpdate();
void rootWindowDestroy();

void viewportWindowInit(EditorPartsCarePackage carePackage);
void viewportWindowUpdate();

void resourcesWindowInit();
void resourcesWindowUpdate();

}

#endif /* SHADOW_NATIVE_EDITOR_EDITOR_PARTS_EDITOR_PARTS_HPP */