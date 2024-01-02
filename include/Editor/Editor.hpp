#ifndef SHADOW_NATIVE_EDITOR_EDITOR_HPP
#define SHADOW_NATIVE_EDITOR_EDITOR_HPP

#include "Core.hpp"
#include "Editor/ProjectBrowser.hpp"
#include "Editor/Project.hpp"
#include "Scene/Scene.hpp"
#include "ShadowWindow.hpp"
#include "bgfx/bgfx.h"
#include <string>

namespace Shadow {

struct EditorPartsCarePackage {
	ShadowWindow* editorWindowRef;
	bgfx::TextureHandle* viewportTextureRef;
	Reference<Shadow::Scene> editorSceneRef;
	float* viewportViewMatrix;
	float* viewportProjectionMatrix;

	EditorPartsCarePackage() { }

	EditorPartsCarePackage(
		ShadowWindow* editorWindowRef,
		bgfx::TextureHandle* viewportTextureRef,
		Reference<Shadow::Scene> editorSceneRef,
		float* viewportViewMatrix,
		float* viewportProjectionMatrix
	)
		: editorWindowRef(editorWindowRef)
		, viewportTextureRef(viewportTextureRef)
		, editorSceneRef(editorSceneRef)
		, viewportViewMatrix(viewportViewMatrix)
		, viewportProjectionMatrix(viewportProjectionMatrix) { }
};

void loadScene(const std::string& sceneFilePath);

int startEditor(Shadow::Editor::ProjectEntry project);

}

#endif /* SHADOW_NATIVE_EDITOR_EDITOR_HPP */