#include "Core.hpp"
#include "GLFW/glfw3.h"
#include "ProductionRuntime/RuntimeBootstrapper.hpp"
#include "Scene/Scene.hpp"
#include "ShadowWindow.hpp"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bx/bx.h"

#if BX_PLATFORM_OS_DESKTOP

#define TEMP_VIEW_ID 0

namespace Shadow {

int StartProductionRuntime() {

	ShadowWindow window(1280, 720, "WIS");
	
	bgfx::Init init;
	init.type = bgfx::RendererType::Vulkan;
	init.platformData.ndt = window.getNativeDisplayHandle();
	init.platformData.nwh = window.getNativeWindowHandle();
	
	init.resolution.width = (uint32_t)window.getExtent().width;
	init.resolution.width = (uint32_t)window.getExtent().height;
	init.resolution.reset = BGFX_RESET_VSYNC;
	bgfx::init(init);

	bgfx::setViewClear(TEMP_VIEW_ID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x111111FF, 1.0f, 0);
	bgfx::setViewRect(TEMP_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);

	Reference<Scene> scene = CreateReference<Scene>();
	

	while (!window.shouldClose()) {
		glfwPollEvents();

		if (window.wasWindowResized()) {
			bgfx::reset(
				(uint32_t)window.getExtent().width, (uint32_t)window.getExtent().height, BGFX_RESET_VSYNC);
			bgfx::setViewRect(TEMP_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);

			window.resetWindowResizedFlag();
		}

		bgfx::frame();
	}

	bgfx::shutdown();
	window.shutdown();

	return 0;
}

}

#endif