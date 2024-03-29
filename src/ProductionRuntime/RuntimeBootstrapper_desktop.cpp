#include "Core.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "Mouse.hpp"
#include "ProductionRuntime/RuntimeBootstrapper.hpp"
#include "RenderBootstrapper.hpp"
#include "Scene/FlyCamera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneSerializer.hpp"
#include "ShadowWindow.hpp"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bx/bx.h"
#include "Keyboard.hpp"
#include "imgui/theme.hpp"
#include "imgui_impl_glfw.h"
#include "imgui/imgui_impl_bgfx.h"

#if BX_PLATFORM_OS_DESKTOP

#define TEMP_VIEW_ID 0

namespace Shadow {

int StartProductionRuntime() {

	ShadowWindow window(1280, 720, "WIS");

	Mouse mouse(&window);
	Keyboard keyboard(&window);

	RenderBootstrapper rb(&window, bgfx::RendererType::Vulkan, false);

	Reference<Scene> scene = CreateReference<Scene>();
	SceneSerializer ss(scene);
	ss.deserialize("./default.scene");

	SceneNamespace::FlyCamera flyCamera(&mouse, &keyboard);
	flyCamera.setPosition({0.0f, 0.0f, -15.0f });
	flyCamera.setVerticalAngle(0.0f);

	while (!window.shouldClose()) {
		window.pollEvents();

		rb.startFrame();
		
		scene->onUpdate(TEMP_VIEW_ID);

		mouse.onUpdate();

		EditorConsole::Frontend::onUpdate();

		bgfx::setDebug(BGFX_DEBUG_STATS);
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(3, 1, 0x01, "SHADOW ENGINE");
		bgfx::dbgTextPrintf(3, 2, 0x01, "Scene: %s", scene->sceneName.c_str());

		rb.endFrame();
	}

	rb.shutdown();
	window.shutdown();

	return 0;
}

}

#endif