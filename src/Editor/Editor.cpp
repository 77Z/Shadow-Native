#include "Editor/Editor.hpp"
#include "Debug/Logger.h"
#include "Editor/ContentBrowser.hpp"
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
#include <GLFW/glfw3.h>
#include <csignal>
#include <cstdint>
#include <imgui/imgui_impl_bgfx.h>

#define VIEWPORT_VIEW_ID 10

static Shadow::ShadowWindow* refWindow;

static std::string gblProjectDir;

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

static void drawMainMenuBar() {
	if (ImGui::BeginMainMenuBar()) {

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

		ImGui::EndMainMenuBar();
	}
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

	ImGui::Text("%s", gblProjectDir.c_str());
	ImGui::Text("Mouse over viewport: %s", mouseOverVport ? "true" : "false");

	ImGui::Text("vportWidth: %f vportHeight: %f", vportWidth, vportHeight);

	ImGui::End();
}

static void drawEditorWindows() {
	ImGui::ShowDemoWindow();
	drawViewportWindow();
	drawDebugWindow();
}

static void sigintHandler(int signal) { refWindow->close(); }

namespace Shadow {

int startEditor(std::string projectDir) {

	InitBXFilesystem();
	IMGUI_CHECKVERSION();

	gblProjectDir = projectDir;

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

	/////////////////////////////////////////////

	bgfx::ProgramHandle program = loadProgram("test/vs_test.vulkan", "test/fs_test.vulkan");
	Scene scene;
	auto cube = scene.createEntity("Cube");
	cube.addComponent<CubeComponent>(0.0f);

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
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		projectPreferencesPanel.onUpdate();
		drawMainMenuBar();
		drawEditorWindows();
		contentBrowser.onUpdate();

		ImGui::Begin("AHHHHH");

		if (ImGui::Button("Open modal"))
			projectPreferencesPanel.open();

		if (ImGui::Button("LOAD USERCODE BRAZAA")) {
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