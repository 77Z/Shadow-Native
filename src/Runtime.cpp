#include "Runtime.h"
#include "Chunker/Chunker.hpp"
// #include "Chunker/ChunkerDevUI.hpp"
#include "Components/Camera.h"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.h"
#include "Debug/Profiler.hpp"
#include "Editor/ContentBrowser.hpp"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneExplorer.hpp"
#include "ShadowWindow.h"
#include "UI/Font.h"
#include "UI/ShadowFlinger.hpp"
#include "UserCode.h"
#include "UserInput.h"
#include "bgfx/defines.h"
#include "bx/platform.h"
#include "bx/timer.h"
#include "db.h"
#include "imgui/imgui_utils.h"
#include "imgui/theme.h"
#include "shadow/Mesh.h"
#include "shadow/audio.h"
#include "types.h"
#include <Util.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <bx/math.h>
#include <cstdint>
#include <generated/autoconf.h>
#include <string>

#define GLM_FORCE_RADIANS
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "imgui/imgui_memory_editor.h"
#include <GLFW/glfw3.h>
#include <bgfx/bgfx.h>

#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3native.h>
#include <bgfx/platform.h>
#include <imgui.h>
#include <imgui/imgui_impl_bgfx.h>
#include <imgui_impl_glfw.h>

/*#ifdef SHADOW_DEBUG_BUILD
#define IMGUI_VULKAN_DEBUG_REPORT
#endif*/

#define EDITOR_BG_VIEW_ID 0
#define IMGUI_VIEW_ID 10
#define SCENE_VIEW_ID 50
#define SHADOW_FLINGER_VIEW_ID 51

static bool s_showStats = false;
static bool s_showWarningText = true;
static bool s_cameraFly = true;
static bool s_showDemoWindow = false;
static bool vsync = true;
static bool mouseOverViewport = false;

float fov = 60.0f;

/*
float camX = 0.0f;
float camY = 0.0f;
float camZ = 0.0f;
static bool key_forwards_pressed = false;
static bool key_backwards_pressed = false;
static bool key_left_pressed = false;
static bool key_right_pressed = false;
static bool key_up_pressed = false;
static bool key_down_pressed = false;

static void glfw_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
		s_showWarningText = !s_showWarningText;

	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
		s_showStats = !s_showStats;

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		key_forwards_pressed = true;
	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		key_forwards_pressed = false;

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		key_backwards_pressed = true;
	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		key_backwards_pressed = false;

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		key_left_pressed = true;
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		key_left_pressed = false;

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		key_right_pressed = true;
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		key_right_pressed = false;

	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		key_up_pressed = true;
	if (key == GLFW_KEY_E && action == GLFW_RELEASE)
		key_up_pressed = false;

	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		key_down_pressed = true;
	if (key == GLFW_KEY_Q && action == GLFW_RELEASE)
		key_down_pressed = false;
}*/

// void handle_sigint(int signal) { WARN("Recieved SIGINT"); }

int Shadow::StartRuntime() {
	InitBXFilesystem();
	RAPID_PROFILE_INIT();
	INTERVAL(STARTUP);

	// int width = 1280;
	// int height = 720;

	int width = 1800;
	int height = 1000;

	ShadowWindow shadowWindow(width, height, CONFIG_PRETTY_NAME);
	Audio::AudioEngine audio;
	// Chunker::ChunkerDevUI chunkerDevUI;

	IMGUI_CHECKVERSION();

	Shadow::UserInput::initMouse(shadowWindow.window);

	bgfx::Init init;

	init.type = bgfx::RendererType::OpenGL;

#if BX_PLATFORM_WINDOWS
	init.platformData.ndt = NULL;
	init.platformData.nwh = (void*)(uintptr_t)glfwGetWin32Window(shadowWindow.window);
#elif BX_PLATFORM_LINUX
	init.platformData.ndt = glfwGetX11Display();
	init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(shadowWindow.window);
#endif

	auto bounds = shadowWindow.getExtent();
	width = bounds.width;
	height = bounds.height;
	init.resolution.width = (uint32_t)width;
	init.resolution.height = (uint32_t)height;
	init.resolution.reset = vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;
	if (!bgfx::init(init))
		return 1;

	bgfx::setViewClear(EDITOR_BG_VIEW_ID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x222222FF, 1.0f, 0);
	bgfx::setViewRect(EDITOR_BG_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);

	// Set view 0 to be the same dimensions as the window and to clear the color buffer
	bgfx::setViewClear(SCENE_VIEW_ID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x222222FF, 1.0f, 0);
	bgfx::setViewRect(SCENE_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);

	// ImGui init
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigDockingTransparentPayload = true;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	// Load DebugUI fonts
	// io.Fonts->AddFontDefault();

	// Loading font from Chunker works but causes runtime error on close
	// Chunker::FileIndex imguiChunkFileIndex = Chunker::indexChunk("./imgui.chunk");
	// std::string nerdFont = Chunker::readFile(imguiChunkFileIndex, "imgui.chunk/nerdfont.ttf");
	// io.Fonts->AddFontFromMemoryTTF(nerdFont.data(), nerdFont.size(), 16.0f);

	io.Fonts->AddFontFromFileTTF("./caskaydia-cove-nerd-font-mono.ttf", 16.0f);
	io.FontGlobalScale = 1.3f;
	io.IniFilename = "devui.ini";

	ImGui::SetupTheme();

	ImGui_Implbgfx_Init(IMGUI_VIEW_ID);
	ImGui_ImplGlfw_InitForVulkan(shadowWindow.window, true);

	// Init stuffs

	const bgfx::Caps* rendererCapabilities = bgfx::getCaps();

	Shadow::Camera camera;
	camera.distance(10.0f);

	// Yes, this causes a memory leak. Too bad!
	Shadow::Mesh mesh("suzanne.mesh");
	// Shadow::EditorConsoleManager consoleManager;
	// consoleManager.createNewConsole("Mesh Debug Console");
	// consoleManager.createNewConsole("General Purpose Console");

	// Shadow::UserCode userCode;

	// User Settings

	Database userSettingsDB("./usersettings");
	std::string readFOV = userSettingsDB.read("FOV");
	if (readFOV.empty())
		userSettingsDB.write("FOV", std::to_string(60.0f));
	fov = std::stof(userSettingsDB.read("FOV"));

	std::string nameDemo = userSettingsDB.read("Name");
	if (nameDemo.empty())
		userSettingsDB.write("Name", "Blank name...");

	// int64_t m_timeOffset;
	bgfx::UniformHandle u_time = bgfx::createUniform("u_time", bgfx::UniformType::Vec4);

	bgfx::ProgramHandle program = loadProgram("test/vs_test.vulkan", "test/fs_test.vulkan");
	bgfx::ProgramHandle meshProgram = loadProgram("mesh/vs_mesh.vulkan", "mesh/fs_mesh.vulkan");

	float speed = 0.37f;
	float time = 0.0f;

	UI::ShadowFlingerViewport shadowFlinger(SHADOW_FLINGER_VIEW_ID);

	Shadow::Scene activeScene;
	Shadow::SceneExplorer sceneExplorer(activeScene);
	Shadow::ContentBrowser contentBrowser;

	auto dummyEntity = activeScene.createEntity("Dummy");
	entt::entity eneenen = dummyEntity;
	auto demoCube = activeScene.createEntity("Primary Cube");
	auto cubeTwo = activeScene.createEntity("Cube Two!");
	// auto another = activeScene.createEntity("Another Cube");
	// auto againn = activeScene.createEntity("Cube Again");

	demoCube.addComponent<CubeComponent>(5.0f);
	cubeTwo.addComponent<CubeComponent>(-5.0f);
	// another.addComponent<CubeComponent>();
	// againn.addComponent<CubeComponent>(2.0f);

	auto& transform = demoCube.GetComponent<TransformComponent>();
	// transform.translation.z = 3.0f;
	transform.translation.x = 3.0f;
	transform.translation.y = 1.2f;
	transform.rotation.x = 5.0f;
	transform.rotation.y = 8.0f;

	float cubeMtx[16];
	bx::mtxSRT(cubeMtx, 3.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	std::string outStr = "[ ";
	for (float item : cubeMtx) {
		outStr.append(std::to_string((int)item));
		outStr.append(" ");
	}
	outStr.append("]");

	WARN("%s", outStr.c_str());

	// activeScene.Reg().emplace<TransformComponent>(demoCube, cubeMtx);
	// activeScene.Reg().emplace<ShapePusherComponent>(demoCube, 0xFF00FF00);

	INTERVAL_END(STARTUP);

	while (!shadowWindow.shouldClose()) {
		glfwPollEvents();

		INTERVAL(GAMELOOP);

		if (shadowWindow.wasWindowResized()) {
			auto bounds = shadowWindow.getExtent();
			width = bounds.width;
			height = bounds.height;
			bgfx::reset(
				(uint32_t)width, (uint32_t)height, vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
			bgfx::setViewRect(SCENE_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);
			bgfx::setViewRect(EDITOR_BG_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);
			shadowFlinger.resized();
			shadowWindow.resetWindowResizedFlag();
		}

		Shadow::UserInput::updateMouse();

		// TODO: ImGui should probably be disabled for production use
		// #ifndef SHADOW_DEBUG_BUILD

		// ImGui
		ImGui_Implbgfx_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		if (s_showDemoWindow)
			ImGui::ShowDemoWindow();

		// static MemoryEditor memedit;
		// memedit.DrawWindow("Memory Editor", &memedit, sizeof(memedit));

		sceneExplorer.onUpdate(eneenen);
		contentBrowser.onUpdate();

		// consoleManager.onUpdate();

		// chunkerDevUI.drawUI();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin(
			"Viewport", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground);
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		mouseOverViewport = ImGui::IsWindowHovered();

		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;

		ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255));
		ImGui::End();

		ImGui::PopStyleVar(1);

		ImGui::Begin(CONFIG_PRETTY_NAME);

		ImGui::Checkbox("Show Stats (F3)", &s_showStats);
		ImGui::Checkbox("Show warning text (F1)", &s_showWarningText);
		ImGui::Checkbox("Camera Fly", &s_cameraFly);
		ImGui::Text("ImGui Wants Mouse: %s", ImGui::MouseOverArea() ? "true" : "false");
		ImGui::Text("Mouse over viewport: %s", mouseOverViewport ? "true" : "false");
		ImGui::Text("MOUSE X: %i Y: %i", (int)UserInput::getMouseX(), (int)UserInput::getMouseY());
		ImGui::Text("Mouse Diff X: %i Y: %i", (int)UserInput::getMouseXDiff(),
			(int)UserInput::getMouseYDiff());
		ImGui::Text("Window Extents: W: %i, H: %i", width, height);
		ImGui::SliderFloat("FOV", &fov, 0.0f, 180.0f);
		// ImGui::InputText("Name Demo", &nameDemo);
		ImGui::Checkbox("Vsync", &vsync);
		if (ImGui::Button("Close Engine"))
			shadowWindow.close();
		ImGui::SameLine();
		if (ImGui::Button("Open Dev UI Demo"))
			s_showDemoWindow = !s_showDemoWindow;

		ImGui::SeparatorText("Audio");
		if (ImGui::Button("Play Demo Audio"))
			audio.playTestAudio();

		ImGui::SeparatorText("UserCode");
		// if (ImGui::Button("Reload UserCode Library"))
		//			userCode.reload();
		ImGui::SeparatorText("Entities & Actors");
		ImGui::Text("%s", demoCube.GetComponent<TagComponent>().tag.c_str());

		ImGui::End();

		ImGui::Render();

		ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

		// This dummy draw call is here to make sure that view 0 is cleared if no other draw
		// calls are submitted to view 0
		bgfx::touch(SCENE_VIEW_ID);

#ifdef SHADOW_DEBUG_BUILD
		const bgfx::Stats* stats = bgfx::getStats();
		bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
#endif

		// Use debug font to print information about this example.
		bgfx::dbgTextClear();

		if (s_showWarningText) {
			bgfx::dbgTextPrintf(3, 2, 0x01, "DEBUG BUILD OF %s",
				boost::to_upper_copy<std::string>(CONFIG_PRETTY_NAME).c_str());
			bgfx::dbgTextPrintf(3, 3, 0x01, "NOT READY FOR PRODUCTION");

#ifdef CONFIG_VINCES_MORE_VERBOSE_DEBUG_TEXT

			uint16_t line = 5;
			bgfx::dbgTextPrintf(3, line++, 0xf0,
				BX_PLATFORM_NAME " " BX_CPU_NAME " " BX_ARCH_NAME " " BX_COMPILER_NAME);

#ifdef CONFIG_DYNAMIC_USERCODE_LIBRARY
			bgfx::dbgTextPrintf(3, line++, 0xf0, "Dynamic UserCode Loading Enabled");

#endif

			bgfx::dbgTextPrintf(3, line++, 0xf0, "%i", stats->numViews);

#endif
		}

		int64_t now = bx::getHPCounter();
		static int64_t last = now;
		const int64_t frameTime = now - last;
		last = now;
		const double freq = double(bx::getHPFrequency());
		const float deltaTime = float(frameTime / freq);

		time += (float)(frameTime + speed / freq);

		bgfx::setUniform(u_time, &time);

		// EDITOR MATRIX

		const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
		const bx::Vec3 eye = { 0.0f, 0.0f, 0.0f };
		float editorViewMatrix[16];
		bx::mtxLookAt(editorViewMatrix, eye, at);
		float editorProjectionMatrix[16];
		bx::mtxProj(editorProjectionMatrix, 60.0f, float(width) / float(height), 0.1f, 100.0f,
			bgfx::getCaps()->homogeneousDepth);
		bgfx::setViewTransform(EDITOR_BG_VIEW_ID, editorViewMatrix, editorProjectionMatrix);

		bgfx::touch(EDITOR_BG_VIEW_ID);

		////////

		float viewMatrix[16];
		camera.update(deltaTime);
		camera.mtxLookAt(viewMatrix);

		float projectionMatrix[16];
		const float aspectRatio = float(vMax.x - vMin.x) / float(vMax.y - vMin.y);
		bx::mtxProj(projectionMatrix, fov, aspectRatio, 0.01f, 1000.0f,
			rendererCapabilities->homogeneousDepth);

		bgfx::setViewTransform(SCENE_VIEW_ID, viewMatrix, projectionMatrix);
		bgfx::setViewRect(
			SCENE_VIEW_ID, vMin.x, vMin.y, uint16_t(vMax.x - vMin.x), uint16_t(vMax.y - vMin.y));

		bgfx::touch(SCENE_VIEW_ID);

		float orientation[16];
		bx::mtxIdentity(orientation);
		bgfx::setTransform(orientation);

		if (Shadow::UserInput::isLeftMouseDown() && mouseOverViewport)
			camera.orbit(UserInput::getMouseXDiff() / 1000, UserInput::getMouseYDiff() / 1000);

		if (Shadow::UserInput::isRightMouseDown() && mouseOverViewport)
			camera.dolly(UserInput::getMouseYDiff() / 1000);

		/*
		if (key_backwards_pressed)
			camZ -= 0.3f;
		if (key_forwards_pressed)
			camZ += 0.3f;
		if (key_left_pressed)
			camX += 0.3f;
		if (key_right_pressed)
			camX -= 0.3f;
		if (key_up_pressed)
			camZ += 0.3f;
		if (key_down_pressed)
			camZ -= 0.3f;
		*/

		activeScene.onUpdate(program);

		shadowFlinger.draw(program, width, height);

		// float meshMtx[16];
		// mesh.submit(SCENE_VIEW_ID, meshProgram, meshMtx);

		/* bgfx::setTransform(cubeMtx, 1);
		bgfx::setVertexBuffer(0, vbh);
		bgfx::setIndexBuffer(ibh);

		bgfx::submit(SCENE_VIEW_ID, program); */

		bgfx::frame();

		INTERVAL_END(GAMELOOP);
	}

	userSettingsDB.write("FOV", std::to_string(fov));
	userSettingsDB.write("Name", nameDemo);

	// bgfx::destroy(ibh);
	// bgfx::destroy(vbh);
	bgfx::destroy(program);
	bgfx::destroy(meshProgram);
	bgfx::destroy(u_time);

	shadowFlinger.unload();

	contentBrowser.unload();

	mesh.unload();

	ImGui_ImplGlfw_Shutdown();
	ImGui_Implbgfx_Shutdown();

	ShutdownBXFilesytem();

	Shadow::ShutdownRuntime();

	return 0;
}

void Shadow::ShutdownRuntime() {
	// * Most of the classes shut themselves down at
	// * this point, which happens after this step
	ImGui::DestroyContext();
	bgfx::shutdown();

	PRINT("Goodbye from Shadow Engine");
}
