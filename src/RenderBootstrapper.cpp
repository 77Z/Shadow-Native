#include "RenderBootstrapper.hpp"
#include "Debug/Logger.hpp"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "imgui.h"
#include "imgui/theme.hpp"
#include "imgui_impl_glfw.h"
#include <cstdint>
#include <imgui/imgui_impl_bgfx.h>

namespace Shadow {

RenderBootstrapper::RenderBootstrapper(
	ShadowWindow* window, bgfx::RendererType::Enum renderer, bool vsync)
	: window(window)
	, vsync(vsync) {
	IMGUI_CHECKVERSION();

	bgfx::Init init;
	init.type = renderer;
	init.platformData.ndt = window->getNativeDisplayHandle();
	init.platformData.nwh = window->getNativeWindowHandle();

	auto bounds = window->getExtent();
	init.resolution.width = (uint32_t)bounds.width;
	init.resolution.height = (uint32_t)bounds.height;
	init.resolution.reset = vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;

	bgfx::init(init);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigDockingTransparentPayload = true;

	io.Fonts->AddFontFromFileTTF("./Resources/caskaydia-cove-nerd-font-mono.ttf", 16.0f);
	io.Fonts->AddFontDefault();
	io.FontGlobalScale = 1.5f;
	std::string iniFile = "./Resources/" + window->windowTitle + ".ini";
	io.IniFilename = iniFile.c_str();

	ImGui::SetupTheme();

	ImGui_Implbgfx_Init(255);
	ImGui_ImplGlfw_InitForVulkan(window->window, true);
}

void RenderBootstrapper::startFrame() {

	if (window->wasWindowResized()) {
		auto bounds = window->getExtent();
		bgfx::reset((uint32_t)bounds.width, (uint32_t)bounds.height,
			vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
		bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);
		window->resetWindowResizedFlag();
	}

	ImGui_Implbgfx_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void RenderBootstrapper::endFrame() {
	ImGui::Render();
	ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

	bgfx::frame();
}

void RenderBootstrapper::shutdown() {
	ImGui_Implbgfx_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
	bgfx::shutdown();
}

}