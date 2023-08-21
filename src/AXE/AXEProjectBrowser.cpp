#include "AXE/AXEProjectBrowser.hpp"
#include "GLFW/glfw3.h"
#include "RenderBootstrapper.hpp"
#include "ShadowWindow.hpp"
#include "Util.hpp"
#include "bgfx/bgfx.h"
#include "imgui.h"

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
	ShadowWindow axeProjectBrowserWindow(1000, 720, "AXE Projects");
	RenderBootstrapper rb(&axeProjectBrowserWindow, bgfx::RendererType::Vulkan);

	while (!axeProjectBrowserWindow.shouldClose()) {
		glfwPollEvents();

		rb.startFrame();

		drawRootWindow();

		rb.endFrame();
	}

	rb.shutdown();

	return 0;
}

}
