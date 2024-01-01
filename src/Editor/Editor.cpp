#include "Core.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Project.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneSerializer.hpp"
#include "ShadowWindow.hpp"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include <csignal>
#include <cstdint>


#define EDITOR_UI_VIEW_ID 2
#define EDITOR_VIEWPORT_VIEW_ID 10

static bool vsync = false;

static Shadow::ShadowWindow* editorWindowReference;
static Shadow::Reference<Shadow::Scene> editorScene;

static void sigintHandler(int signal) {
	BX_UNUSED(signal);
	editorWindowReference->close();
}

static void resetViews() {
	bgfx::setViewClear(EDITOR_VIEWPORT_VIEW_ID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x111111FF, 1.0f, 0);
	bgfx::setViewClear(EDITOR_UI_VIEW_ID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x111111FF, 1.0f, 0);
	bgfx::setViewRect(EDITOR_VIEWPORT_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);
	bgfx::setViewRect(EDITOR_UI_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);
}

namespace Shadow {

void loadScene(const std::string &sceneFilePath) {
	SceneSerializer ss(editorScene);
	ss.deserialize(sceneFilePath);
}

int startEditor(Shadow::Editor::ProjectEntry project) {
	Editor::setCurrentProjectName(project.name);
	Editor::setCurrentProjectPath(project.path);

	ShadowWindow editorWindow(1800, 900, "Shadow Editor");
	editorWindowReference = &editorWindow;
	std::signal(SIGINT, sigintHandler);


	bgfx::Init init;
	init.type = bgfx::RendererType::Vulkan;
	init.platformData.ndt = editorWindow.getNativeDisplayHandle();
	init.platformData.nwh = editorWindow.getNativeWindowHandle();
	init.resolution.width = (uint32_t)editorWindow.getExtent().width;
	init.resolution.width = (uint32_t)editorWindow.getExtent().height;
	init.resolution.reset = vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;
	bgfx::init(init);

	resetViews();

	editorScene = Shadow::CreateReference<Shadow::Scene>();

	while (!editorWindow.shouldClose()) {
		editorWindow.pollEvents();

		if (editorWindow.wasWindowResized()) {
			bgfx::reset(
				(uint32_t)editorWindow.getExtent().width,
				(uint32_t)editorWindow.getExtent().height,
				vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
			
			resetViews();

			editorWindow.resetWindowResizedFlag();
		}

		bgfx::frame();
	}

	editorScene->unload();

	bgfx::shutdown();
	editorWindow.shutdown();

	return 0;
}

}