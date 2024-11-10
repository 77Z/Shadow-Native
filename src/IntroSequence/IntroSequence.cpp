#include "Core.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "Mesh.hpp"
#include "Mouse.hpp"
#include "ProductionRuntime/RuntimeBootstrapper.hpp"
#include "RenderBootstrapper.hpp"
#include "Scene/FlyCamera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneSerializer.hpp"
#include "ShadowWindow.hpp"
#include "Util.hpp"
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

int StartProductionRuntimeWHAT() {

	ShadowWindow window(1280, 720, "WIS", true, true);

	Mouse mouse(&window);
	Keyboard keyboard(&window);

	RenderBootstrapper rb(&window, bgfx::RendererType::Vulkan, TEMP_VIEW_ID, false);

	Reference<Scene> scene = CreateReference<Scene>();
	SceneSerializer ss(scene);
	if (!ss.deserialize("/home/vince/.config/Shadow/Projects/WIS/Content/mesh.scene")) {
		ERROUT("FAILED TO LOAD SCENE");
		return 1;
	}

	// SceneNamespace::FlyCamera flyCamera(&mouse, &keyboard);
	// flyCamera.setPosition({0.0f, 0.0f, -15.0f });
	// flyCamera.setVerticalAngle(0.0f);

	//TODO: REMOVE
	bgfx::ProgramHandle meshPgrm = loadProgram("mesh/vs_mesh.sc.spv", "mesh/fs_mesh.sc.spv");

	while (!window.shouldClose()) {
		window.pollEvents();

		rb.startFrame();
		
		scene->onUpdate(TEMP_VIEW_ID);

		// float mtx[16];
		// bx::mtxSRT(mtx, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.0f);

		//: BRUH {{{



		//:  }}}

		
		// basic camera
		const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
        const bx::Vec3 eye = {0.0f, 0.0f, -5.0f};
        float view[16];
        bx::mtxLookAt(view, eye, at);
        float proj[16];
        bx::mtxProj(proj, 90.0f, float(window.getExtent().width) / float(window.getExtent().height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);

		mouse.onUpdate();

		EditorConsole::Frontend::onUpdate();

		bgfx::setDebug(BGFX_DEBUG_STATS);
		// bgfx::dbgTextClear();
		// bgfx::dbgTextPrintf(3, 1, 0x01, "SHADOW ENGINE");
		// bgfx::dbgTextPrintf(3, 2, 0x01, "Scene: %s", scene->sceneName.c_str());

		rb.endFrame();
	}

	bgfx::destroy(meshPgrm);
	scene->unload();
	rb.shutdown();
	window.shutdown();

	return 0;
}

}

#endif
