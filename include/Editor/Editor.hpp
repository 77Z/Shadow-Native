#ifndef SHADOW_NATIVE_EDITOR_EDITOR_HPP
#define SHADOW_NATIVE_EDITOR_EDITOR_HPP

#include "Editor/ProjectBrowser.hpp"
#include "Editor/Project.hpp"
#include <string>

namespace Shadow {

void loadScene(const std::string& sceneFilePath);

int startEditor(Shadow::Editor::ProjectEntry project);

}

#endif /* SHADOW_NATIVE_EDITOR_EDITOR_HPP */