#include "AXE/AXEProjectBrowser.hpp"
#include "GLFW/glfw3.h"
#include "ShadowWindow.hpp"
#include "Util.hpp"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "imgui.h"
#include "imgui/theme.hpp"
#include "imgui_impl_glfw.h"
#include <cstdint>
#include <imgui/imgui_impl_bgfx.h>

namespace Shadow::AXE {

static void drawRootWindow() {

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_MenuBar;

	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 255));

	ImGui::Begin("RootWindow", nullptr, window_flags);

	ImGui::PopStyleColor();

	ImGui::PopStyleVar(3);

	ImGui::Text("Hello");

	ImGui::End();
}

int startAXEProjectBrowser() {
	InitBXFilesystem();
	IMGUI_CHECKVERSION();

	int width = 1000, height = 720;

	ShadowWindow axeProjectBrowserWindow(width, height, "AXE Projects");

	bgfx::Init init;
	init.type = bgfx::RendererType::Vulkan;
	init.platformData.ndt = axeProjectBrowserWindow.getNativeDisplayHandle();
	init.platformData.nwh = axeProjectBrowserWindow.getNativeWindowHandle();

	init.resolution.width = (uint32_t)width;
	init.resolution.height = (uint32_t)height;
	init.resolution.reset = BGFX_RESET_VSYNC;

	if (!bgfx::init(init))
		return 1;

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x222222FF, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigDockingTransparentPayload = true;

	io.Fonts->AddFontFromFileTTF("./Resources/caskaydia-cove-nerd-font-mono.ttf", 16.0f);
	io.Fonts->AddFontDefault();
	io.FontGlobalScale = 1.5f;
	io.IniFilename = "./Resources/axeProjBrowser.ini";

	ImGui::SetupTheme();

	ImGui_Implbgfx_Init(255);
	ImGui_ImplGlfw_InitForVulkan(axeProjectBrowserWindow.window, true);

	while (!axeProjectBrowserWindow.shouldClose()) {
		glfwPollEvents();

		if (axeProjectBrowserWindow.wasWindowResized()) {
			auto bounds = axeProjectBrowserWindow.getExtent();
			width = bounds.width;
			height = bounds.height;
			bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
			bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);
			axeProjectBrowserWindow.resetWindowResizedFlag();
		}

		ImGui_Implbgfx_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();

		drawRootWindow();

		ImGui::Render();
		ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

		bgfx::frame();
	}

	ImGui_Implbgfx_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ShutdownBXFilesytem();

	ImGui::DestroyContext();
	bgfx::shutdown();

	return 0;
}

}
