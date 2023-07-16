#include "Editor/ProjectBrowser.hpp"
#include "Debug/Profiler.hpp"
#include "Runtime.h"
#include "ShadowWindow.h"
#include "Util.h"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bx/timer.h"
#include "imgui.h"
#include "imgui/theme.h"
#include "imgui_impl_glfw.h"
#include <imgui/imgui_impl_bgfx.h>

#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

static bool vsync = true;
static bool openEditorAfterDeath = false;

static Shadow::ShadowWindow* refWindow;

static ImFont* mainFont;
static ImFont* headingFont;

static void drawProjectBrowser() {
	ImGui::Begin("Project Browser");

	ImGui::PushFont(headingFont);
	ImGui::Text("Projects");
	ImGui::Separator();
	ImGui::PopFont();

	if (ImGui::Button("New Project"))
		ImGui::OpenPopup("New Project");

	ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiCond_FirstUseEver);
	if (ImGui::BeginPopupModal("New Project", nullptr, ImGuiWindowFlags_NoTitleBar)) {
		ImGui::PushFont(headingFont);
		ImGui::Text("New Project");
		ImGui::Separator();
		ImGui::PopFont();
		ImGui::EndPopup();
	}

	if (ImGui::Button("Force open editor")) {
		openEditorAfterDeath = true;
		refWindow->close();
	}

	ImGui::End();
}

namespace Shadow {

int Editor::startProjectBrowser() {
	InitBXFilesystem();
	IMGUI_CHECKVERSION();

	int width = 1280, height = 720;

	ShadowWindow projectEditorWindow(width, height, "Project Browser");
	refWindow = &projectEditorWindow;

	bgfx::Init init;
	init.type = bgfx::RendererType::OpenGL;

#if BX_PLATFORM_WINDOWS
	init.platformData.ndt = NULL;
	init.platformData.nwh = (void*)(uintptr_t)glfwGetWin32Window(projectEditorWindow.window);
#elif BX_PLATFORM_LINUX
	init.platformData.ndt = glfwGetX11Display();
	init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(projectEditorWindow.window);
#endif

	auto bounds = projectEditorWindow.getExtent();
	width = bounds.width;
	height = bounds.height;
	init.resolution.width = (uint32_t)width;
	init.resolution.height = (uint32_t)height;
	init.resolution.reset = vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;
	if (!bgfx::init(init))
		return 1;

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x222222FF, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigDockingTransparentPayload = true;

	// TODO: in the future make this one file load into two fonts
	mainFont = io.Fonts->AddFontFromFileTTF("./caskaydia-cove-nerd-font-mono.ttf", 16.0f);
	headingFont = io.Fonts->AddFontFromFileTTF("./caskaydia-cove-nerd-font-mono.ttf", 40.0f);
	io.FontGlobalScale = 1.3f;
	io.IniFilename = "projectBrowser.ini";

	ImGui::SetupTheme();

	ImGui_Implbgfx_Init(255);
	ImGui_ImplGlfw_InitForVulkan(projectEditorWindow.window, true);

	const bgfx::Caps* rendererCapabilities = bgfx::getCaps();

	bgfx::UniformHandle u_time = bgfx::createUniform("u_time", bgfx::UniformType::Vec4);
	float speed = 0.37f;
	float time = 0.0f;

	while (!projectEditorWindow.shouldClose()) {
		glfwPollEvents();

		if (projectEditorWindow.wasWindowResized()) {
			auto bounds = projectEditorWindow.getExtent();
			width = bounds.width;
			height = bounds.height;
			bgfx::reset(
				(uint32_t)width, (uint32_t)height, vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
			bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);
			projectEditorWindow.resetWindowResizedFlag();
		}

		ImGui_Implbgfx_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		ImGui::ShowDemoWindow();

		drawProjectBrowser();

		ImGui::Render();
		ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

		bgfx::touch(0);

		int64_t now = bx::getHPCounter();
		static int64_t last = now;
		const int64_t frameTime = now - last;
		last = now;
		const double freq = double(bx::getHPFrequency());
		const float deltaTime = float(frameTime / freq);

		time += (float)(frameTime + speed / freq);

		bgfx::setUniform(u_time, &time);

		{
			// Blank background matrix
			const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
			const bx::Vec3 eye = { 0.0f, 0.0f, 0.0f };
			float bgViewMatrix[16];
			bx::mtxLookAt(bgViewMatrix, eye, at);
			float bgProjectionMatrix[16];
			bx::mtxProj(bgProjectionMatrix, 60.0f, float(width) / float(height), 0.1f, 100.0f,
				bgfx::getCaps()->homogeneousDepth);
			bgfx::setViewTransform(0, bgViewMatrix, bgProjectionMatrix);

			bgfx::touch(0);
		}

		bgfx::frame();
	}

	bgfx::destroy(u_time);

	ImGui_ImplGlfw_Shutdown();
	ImGui_Implbgfx_Shutdown();

	ShutdownBXFilesytem();

	ImGui::DestroyContext();
	bgfx::shutdown();

	projectEditorWindow.~ShadowWindow();

	if (openEditorAfterDeath) {
		Shadow::StartRuntime();
	}

	return 0;
}

}