#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <bx/bx.h>

#if BX_PLATFORM_LINUX
#	define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#	define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#	define GLFW_EXPOSE_NATIVE_COCOA
#endif

#define GLFW_INCLUDE_VULKAN
#include <bgfx/platform.h>
#include <GLFW/glfw3native.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
//#include <imgui_impl_vulkan.h>
#include <imgui_impl_bgfx.h>

/*#ifdef SHADOW_DEBUG_BUILD
#define IMGUI_VULKAN_DEBUG_REPORT
#endif*/

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

static bool s_showStats = false;

static void glfw_errorCallback(int error, const char *description) {
	fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
		s_showStats = !s_showStats; // Cool trick to toggle
}

int main() {
	glfwSetErrorCallback(glfw_errorCallback);
	if (!glfwInit())
		return 1;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Shadow Engine", nullptr, nullptr);

	IMGUI_CHECKVERSION();

	if (!window)
		return 1;

	glfwSetKeyCallback(window, glfw_keyCallback);

	bgfx::renderFrame();

	bgfx::Init init;

	init.platformData.ndt = glfwGetX11Display();
	init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);

	int width, height; // This can probably be eliminated by WINDOW_WIDTH and WINDOW_HEIGHT

	glfwGetWindowSize(window, &width, &height);
	init.resolution.width = (uint32_t)width;
	init.resolution.height = (uint32_t)height;
	init.resolution.reset = BGFX_RESET_VSYNC;
	if (!bgfx::init(init))
		return 1;

	// set view 0 to the same dimensions as the window and to clear the color buffer
	const bgfx::ViewId kClearView = 0;
	bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
	bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);


	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_Implbgfx_Init(255);

	ImGui_ImplGlfw_InitForVulkan(window, true);

	//bool showDemoWindow = true;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		// Handle window resize
		int oldWidth = width, oldHeight = height;
		glfwGetWindowSize(window, &width, &height);
		if (width != oldWidth || height != oldHeight) {
			bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
			bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
		}

		//IMGUI
		ImGui_Implbgfx_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
		ImGui::Render();
		ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

		//if (showDemoWindow)
		//	ImGui::ShowDemoWindow(&showDemoWindow);

		// This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0
		bgfx::touch(kClearView);
		// Use debug font to print information about this example.
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F3 to toggle stats");

#ifdef SHADOW_DEBUG_BUILD
		bgfx::dbgTextPrintf(0, 1, 0x0f, "WARNING: DEBUG BUILD OF SHADOW ENGINE");
		bgfx::dbgTextPrintf(0, 2, 0x0f, "NOT READY FOR PRODUCTION");
#endif

		const bgfx::Stats *stats = bgfx::getStats();
		bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);

		bgfx::frame();
	}
	bgfx::shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}
