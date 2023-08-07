#include "Editor/Editor.hpp"
#include "Debug/Logger.h"
#include "Editor/ContentBrowser.hpp"
#include "Editor/Project.hpp"
#include "Editor/ProjectBrowser.hpp"
#include "Editor/ProjectPreferencesPanel.hpp"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"
#include "ShadowWindow.h"
#include "UserCode.h"
#include "Util.h"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bx/platform.h"
#include "bx/timer.h"
#include "imgui.h"
#include "imgui/imgui_utils.h"
#include "imgui/theme.h"
#include "imgui_impl_glfw.h"
#include "imgui_internal.h"
#include <GLFW/glfw3.h>
#include <csignal>
#include <cstdint>
#include <imgui/imgui_impl_bgfx.h>

#define VIEWPORT_VIEW_ID 10

static Shadow::ShadowWindow* refWindow;

static bool vsync = true;
static bool openProjectBrowserOnDeath = false;

// Dummy values to be written over later
static ImVec2 vportMin(0, 0);
static ImVec2 vportMax(800, 450);

static bgfx::TextureHandle vportTex;
static bgfx::FrameBufferHandle vportBuf;
static float lastvportWidth = 0.0f, lastvportHeight = 0.0f;
static float vportWidth = vportMax.x - vportMin.x;
static float vportHeight = vportMax.y - vportMin.y;
static bool mouseOverVport = false;
static bool wasViewportResized = false;

static bgfx::TextureHandle shadowLogo;

static void drawMainMenuBar() {
	if (ImGui::myBeginMenuBar()) {
		ImGui::SetCursorPosX(100);

		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Asset", "CTRL + N")) { }
			ImGui::Separator();
			if (ImGui::MenuItem("Engine Preferences", "CTRL + ,")) { }
			ImGui::Separator();
			if (ImGui::MenuItem("Close Project")) {
				openProjectBrowserOnDeath = true;
				refWindow->close();
			}
			if (ImGui::MenuItem("Exit", "CTRL + SHIFT + Q")) {
				refWindow->close();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "CTRL + Z")) { }
			if (ImGui::MenuItem("Redo", "CTRL + Y", false, false)) { }
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL + X")) { }
			if (ImGui::MenuItem("Copy", "CTRL + C")) { }
			if (ImGui::MenuItem("Paste", "CTRL + V")) { }
			ImGui::Separator();
			if (ImGui::MenuItem("Project Preferences", "CTRL + SHIFT + ,")) {
				PRINT("PROJECT PREFERENCES");
				ImGui::OpenPopup("ProjectPreferencesPanel");
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Build")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Run")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

static void drawTitlebar(float& outBarHeight) {
	const float titlebarHeight = 58.0f;
	outBarHeight = titlebarHeight;
	const bool isMaximized = refWindow->isMaximized();
	float titlebarVerticalOffset = isMaximized ? -6.0f : 0.0f;
	const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;
	ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + titlebarVerticalOffset));
	const ImVec2 titlebarMin = ImGui::GetCursorScreenPos();
	const ImVec2 titlebarMax
		= { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - windowPadding.y * 2.0f,
			  ImGui::GetCursorScreenPos().y + titlebarHeight };
	auto* bgDrawList = ImGui::GetBackgroundDrawList();
	auto* fgDrawList = ImGui::GetForegroundDrawList();
	bgDrawList->AddRectFilled(titlebarMin, titlebarMax, IM_COL32(255, 0, 0, 255));
	// DEBUG TITLEBAR BOUNDS
	// fgDrawList->AddRect(titlebarMin, titlebarMax, IM_COL32(255, 255, 0, 255));

	// Logo
	{
		const int logoWidth = 48;
		const int logoHeight = 48;
		const ImVec2 logoOffset(
			16.0f + windowPadding.x, 5.0f + windowPadding.y + titlebarVerticalOffset);
		const ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x,
			ImGui::GetItemRectMin().y + logoOffset.y };
		const ImVec2 logoRectMax = { logoRectStart.x + logoWidth, logoRectStart.y + logoHeight };
		fgDrawList->AddImage(ImGui::toId(shadowLogo, 0, 0), logoRectStart, logoRectMax);
	}

	// Menubar
	drawMainMenuBar();

	// Window title
	{
		const char* title = refWindow->windowTitle.c_str();
		ImVec2 currentCursorPos = ImGui::GetCursorPos();
		ImVec2 textSize = ImGui::CalcTextSize(title);
		fgDrawList->AddText(ImVec2(ImGui::GetWindowWidth() * 0.5f - textSize.x * 0.5f,
								2.0f + windowPadding.y + 6.0f),
			IM_COL32(255, 255, 255, 255), title);
		ImGui::SetCursorPos(currentCursorPos);
	}
}

static void drawRootWindow() {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

	const bool isMaximized = refWindow->isMaximized();

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(
		ImGuiStyleVar_WindowPadding, isMaximized ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_MenuBar;

	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 255));

	ImGui::Begin("RootWindow", nullptr, window_flags);

	ImGui::PopStyleColor();

	ImGui::PopStyleVar(3);

	float titlebarHeight;
	drawTitlebar(titlebarHeight);
	ImGui::SetCursorPosY(titlebarHeight);

	ImGui::DockSpace(ImGui::GetID("MyDockspace"));

	ImGui::End();
}

static void drawViewportWindow() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiCond_FirstUseEver);
	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse);
	{
		vportMin = ImGui::GetWindowContentRegionMin();
		vportMax = ImGui::GetWindowContentRegionMax();

		vportMin.x += ImGui::GetWindowPos().x;
		vportMin.y += ImGui::GetWindowPos().y;
		vportMax.x += ImGui::GetWindowPos().x;
		vportMax.y += ImGui::GetWindowPos().y;

		vportWidth = vportMax.x - vportMin.x;
		vportHeight = vportMax.y - vportMin.y;

		float vportWidthDiff = vportWidth - lastvportWidth;
		float vportHeightDiff = vportHeight - lastvportHeight;

		if (vportWidthDiff != 0 || vportHeightDiff != 0)
			wasViewportResized = true;
		else
			wasViewportResized = false;

		lastvportWidth = vportWidth;
		lastvportHeight = vportHeight;

		// ImGui::GetForegroundDrawList()->AddRect(vportMin, vportMax, IM_COL32(255, 255, 0, 255));
	}

	mouseOverVport = ImGui::IsWindowHovered();

	ImGui::Image(vportTex, ImVec2(vportWidth, vportHeight));

	ImGui::End();
	ImGui::PopStyleVar(2);
}

static void drawDebugWindow() {
	ImGui::Begin("Dbg");

	ImGui::Text("%s", Shadow::Editor::getCurrentProjectName().c_str());
	ImGui::Text("%s", Shadow::Editor::getCurrentProjectPath().c_str());
	ImGui::Text("Mouse over viewport: %s", mouseOverVport ? "true" : "false");

	ImGui::Text("vportWidth: %f vportHeight: %f", vportWidth, vportHeight);

	ImGui::Separator();

	if (ImGui::Button("Minimize")) {
		refWindow->minimize();
	}
	ImGui::SameLine();
	if (ImGui::Button("Maximize")) {
		refWindow->toggleMaximized();
	}
	ImGui::SameLine();
	if (ImGui::Button("Close")) {
		refWindow->close();
	}

	ImGui::End();
}

static void drawEditorWindows() {
	drawRootWindow();
	ImGui::ShowDemoWindow();
	drawViewportWindow();
	drawDebugWindow();
}

static void sigintHandler(int signal) { refWindow->close(); }

namespace Shadow {

int startEditor(Shadow::Editor::ProjectEntry project) {

	InitBXFilesystem();
	IMGUI_CHECKVERSION();

	Editor::setCurrentProjectName(project.name);
	Editor::setCurrentProjectPath(project.path);

	int width = 1800, height = 1000;

	ShadowWindow editorWindow(width, height, "Shadow Editor");
	refWindow = &editorWindow;

	signal(SIGINT, sigintHandler);

	bgfx::Init init;
	init.type = bgfx::RendererType::Vulkan;
	init.platformData.ndt = editorWindow.getNativeDisplayHandle();
	init.platformData.nwh = editorWindow.getNativeWindowHandle();

	init.resolution.width = (uint32_t)width;
	init.resolution.height = (uint32_t)height;
	init.resolution.reset = vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;
	bgfx::init(init);

	bgfx::setViewClear(VIEWPORT_VIEW_ID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x111111FF, 1.0f, 0);
	bgfx::setViewRect(VIEWPORT_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigDockingTransparentPayload = true;

	io.Fonts->AddFontFromFileTTF("./caskaydia-cove-nerd-font-mono.ttf", 16.0f);
	io.Fonts->AddFontDefault();
	// io.FontGlobalScale = 1.3f;
	io.IniFilename = "editor.ini";

	ImGui::SetupTheme();

	ImGui_Implbgfx_Init(2);
	ImGui_ImplGlfw_InitForVulkan(editorWindow.window, true);

	const bgfx::Caps* rendererCaps = bgfx::getCaps();

	bgfx::UniformHandle u_time = bgfx::createUniform("u_time", bgfx::UniformType::Vec4);
	float speed = 0.37f, time = 0.0f;

	vportTex = bgfx::createTexture2D((uint16_t)width, (uint16_t)height, false, 1,
		bgfx::TextureFormat::BGRA8,
		BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
	vportBuf = bgfx::createFrameBuffer(1, &vportTex, true);

	shadowLogo = loadTexture("./logo.png");

	/////////////////////////////////////////////

	bgfx::ProgramHandle program = loadProgram("test/vs_test.vulkan", "test/fs_test.vulkan");
	Scene scene;
	auto cube = scene.createEntity("Cube");
	cube.addComponent<CubeComponent>(0.0f);
	cube.addComponent<TransformComponent>();

	ContentBrowser contentBrowser;
	Editor::ProjectPreferencesPanel projectPreferencesPanel;

	while (!editorWindow.shouldClose()) {
		glfwPollEvents();

		if (editorWindow.wasWindowResized()) {
			auto bounds = editorWindow.getExtent();
			width = bounds.width;
			height = bounds.height;
			bgfx::reset(
				(uint32_t)width, (uint32_t)height, vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
			bgfx::setViewRect(VIEWPORT_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);

			editorWindow.resetWindowResizedFlag();
		}

		if (wasViewportResized) {
			// Resize viewport texture here???
		}

		ImGui_Implbgfx_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// You don't need to manually make a view for the background if you use this
		// ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		drawEditorWindows();

		projectPreferencesPanel.onUpdate();
		contentBrowser.onUpdate();

		ImGui::Begin("AHHHHH");

		if (ImGui::Button("Open modal"))
			projectPreferencesPanel.open();

		if (ImGui::Button("LOAD USERCODE")) {
			UserCode::loadUserCode();
		}

		ImGui::End();

		ImGui::Render();
		ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

		{
			int64_t now = bx::getHPCounter();
			static int64_t last = now;
			const int64_t frameTime = now - last;
			last = now;
			const double freq = double(bx::getHPFrequency());
			const float deltaTime = float(frameTime / freq);
			time += (float)(frameTime + speed / freq);

			bgfx::setUniform(u_time, &time);
		}

		// Matrix
		{
			const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
			const bx::Vec3 eye = { 0.0f, 0.0f, -5.0f };
			float editorViewMatrix[16];
			bx::mtxLookAt(editorViewMatrix, eye, at);
			float editorProjectionMatrix[16];
			bx::mtxProj(editorProjectionMatrix, 60.0f, vportWidth / vportHeight, 0.1f, 100.0f,
				bgfx::getCaps()->homogeneousDepth);

			bgfx::setViewName(VIEWPORT_VIEW_ID, "Viewport");
			bgfx::setViewTransform(VIEWPORT_VIEW_ID, editorViewMatrix, editorProjectionMatrix);
			// bgfx::setViewRect(
			// 	VIEWPORT_VIEW_ID, 10, 10, uint16_t(vportWidth), uint16_t(vportHeight));
			bgfx::setViewFrameBuffer(VIEWPORT_VIEW_ID, vportBuf);

			bgfx::touch(VIEWPORT_VIEW_ID);
		}

		scene.onUpdate(VIEWPORT_VIEW_ID, program);

		bgfx::frame();
	}

	bgfx::destroy(u_time);
	bgfx::destroy(vportBuf);
	bgfx::destroy(program);
	bgfx::destroy(shadowLogo);

	contentBrowser.unload();
	projectPreferencesPanel.unload();

	ImGui_ImplGlfw_Shutdown();
	ImGui_Implbgfx_Shutdown();

	ShutdownBXFilesytem();

	ImGui::DestroyContext();
	bgfx::shutdown();
	editorWindow.shutdown();

	// TODO: Causes loop of opening windows...
	if (openProjectBrowserOnDeath) {
		Editor::startProjectBrowser();
	}

	return 0;
}

}