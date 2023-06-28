#include "Runtime.h"
#include "Components/Camera.h"
#include "KeyboardInput.hpp"
#include "Logger.h"
#include "ShadowWindow.h"
#include "UI/Font.h"
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
#include <bx/file.h>
#include <bx/math.h>
#include <bx/string.h>
#include <csignal>
#include <cstdint>
#include <generated/autoconf.h>
#include <string>

#define GLM_FORCE_RADIANS
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "imgui/imgui_memory_editor.h"
#include <GLFW/glfw3.h>
#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <cstdio>
#include <imgui_internal.h>

// #include <phonon.h>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <vector>

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

#include <unistd.h>

/*#ifdef SHADOW_DEBUG_BUILD
#define IMGUI_VULKAN_DEBUG_REPORT
#endif*/

#define SCENE_VIEW_ID 10
#define EDITOR_BG_VIEW_ID 0

static bool s_showStats = false;
static bool s_showWarningText = true;
static bool s_cameraFly = true;
static bool vsync = false;

float fov = 60.0f;

// float camX = 0.0f;
// float camY = 0.0f;
// float camZ = 0.0f;
// static bool key_forwards_pressed = false;
// static bool key_backwards_pressed = false;
// static bool key_left_pressed = false;
// static bool key_right_pressed = false;
// static bool key_up_pressed = false;
// static bool key_down_pressed = false;

// static void glfw_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
// 	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
// 		s_showWarningText = !s_showWarningText;

// 	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
// 		s_showStats = !s_showStats;

// 	if (key == GLFW_KEY_W && action == GLFW_PRESS)
// 		key_forwards_pressed = true;
// 	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
// 		key_forwards_pressed = false;

// 	if (key == GLFW_KEY_S && action == GLFW_PRESS)
// 		key_backwards_pressed = true;
// 	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
// 		key_backwards_pressed = false;

// 	if (key == GLFW_KEY_A && action == GLFW_PRESS)
// 		key_left_pressed = true;
// 	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
// 		key_left_pressed = false;

// 	if (key == GLFW_KEY_D && action == GLFW_PRESS)
// 		key_right_pressed = true;
// 	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
// 		key_right_pressed = false;

// 	if (key == GLFW_KEY_E && action == GLFW_PRESS)
// 		key_up_pressed = true;
// 	if (key == GLFW_KEY_E && action == GLFW_RELEASE)
// 		key_up_pressed = false;

// 	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
// 		key_down_pressed = true;
// 	if (key == GLFW_KEY_Q && action == GLFW_RELEASE)
// 		key_down_pressed = false;
// }

// static const glm::vec2 SIZE = glm::vec2(1280, 720);

struct Vertex {
	glm::vec3 position;
	u32 color;
};

struct PosColorVertex {
	float x;
	float y;
	float z;
	uint32_t abgr;
};

// static PosColorVertex cubeVertices[] = {
// 	{ -1.0f, 1.0f, 1.0f, 0xff000000 },
// 	{ 1.0f, 1.0f, 1.0f, 0xff0000ff },
// 	{ -1.0f, -1.0f, 1.0f, 0xff00ff00 },
// 	{ 1.0f, -1.0f, 1.0f, 0xff00ffff },
// 	{ -1.0f, 1.0f, -1.0f, 0xffff0000 },
// 	{ 1.0f, 1.0f, -1.0f, 0xff0000ff },
// 	{ -1.0f, -1.0f, -1.0f, 0xffffff00 },
// 	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
// };

static u32 color = 0xff0000ff;

static PosColorVertex cubeVertices[] = {
	{ -1.5f, 1.0f, 1.0f, color },
	{ 1.0f, 1.0f, 1.0f, color },
	{ -1.0f, -1.0f, 1.0f, color },
	{ 1.0f, -1.0f, 1.0f, color },
	{ -1.0f, 1.0f, -1.0f, color },
	{ 1.0f, 1.0f, -1.0f, color },
	{ -1.0f, -1.0f, -1.0f, 0xffff0000 },
	{ 1.0f, -1.0f, -1.0f, 0xffff0000 },
};

static const uint16_t cubeTriList[] = {
	0,
	1,
	2,
	1,
	3,
	2,
	4,
	6,
	5,
	5,
	6,
	7,
	0,
	2,
	4,
	4,
	2,
	6,
	1,
	5,
	3,
	5,
	7,
	3,
	0,
	4,
	1,
	4,
	5,
	1,
	2,
	3,
	6,
	6,
	3,
	7,
};

// std::vector<float> load_input_audio(const std::string filename) {
// 	std::ifstream file(filename.c_str(), std::ios::binary);

// 	file.seekg(0, std::ios::end);
// 	auto filesize = file.tellg();
// 	auto numsamples = static_cast<int>(filesize / sizeof(float));

// 	std::vector<float> inputaudio(numsamples);
// 	file.seekg(0, std::ios::beg);
// 	file.read(reinterpret_cast<char*>(inputaudio.data()), filesize);

// 	return inputaudio;
// }

// void save_output_audio(const std::string filename, std::vector<float> outputaudio) {
// 	std::ofstream file(filename.c_str(), std::ios::binary);
// 	file.write(reinterpret_cast<char*>(outputaudio.data()), outputaudio.size() * sizeof(float));
// }

// void handle_sigint(int signal) { WARN("Recieved SIGINT"); }

int Shadow::StartRuntime() {

	InitBXFilesystem();

	int width = 1280;
	int height = 720;

	ShadowWindow shadowWindow(width, height, CONFIG_PRETTY_NAME);

	// signal(SIGINT, handle_sigint);

	IMGUI_CHECKVERSION();

	ShadowAudio::initAudioEngine();
	Shadow::UserInput::initMouse(shadowWindow.window);

	/*

		IPLContextSettings contextSettings{};
		contextSettings.version = STEAMAUDIO_VERSION;

		IPLContext context = nullptr;
		iplContextCreate(&contextSettings, &context);

		IPLHRTFSettings hrtfSettings{};
		hrtfSettings.type = IPL_HRTFTYPE_DEFAULT;

		IPLAudioSettings audioSettings{};
		audioSettings.samplingRate = 44100;
		audioSettings.frameSize = 1024;

		IPLHRTF hrtf = nullptr;
		iplHRTFCreate(context, &audioSettings, &hrtfSettings, &hrtf);

		IPLBinauralEffectSettings effectSettings{};
		effectSettings.hrtf = hrtf;

		IPLBinauralEffect effect{};
		iplBinauralEffectCreate(context, &audioSettings, &effectSettings, &effect);

		std::vector<float> inputaudio = load_input_audio("Fail_Sound.raw");
		std::vector<float> outputaudio;

		float* inData[] = { inputaudio.data() };

		IPLAudioBuffer inBuffer{};
		inBuffer.numChannels = 1;
		inBuffer.numSamples = audioSettings.frameSize;
		inBuffer.data = inData;

		IPLAudioBuffer outBuffer{};
		iplAudioBufferAllocate(context, 2, audioSettings.frameSize, &outBuffer);

		std::vector<float> outputaudioframe(2 * audioSettings.frameSize);

		int numframes = inputaudio.size() / audioSettings.frameSize;

		for (int i = 0; i < numframes; i++) {
			IPLBinauralEffectParams effectParams{};
			effectParams.direction = IPLVector3{1.0f, 1.0f, 1.0f};
			effectParams.interpolation = IPL_HRTFINTERPOLATION_NEAREST;
			effectParams.spatialBlend = 1.0f;
			effectParams.hrtf = hrtf;

			iplBinauralEffectApply(effect, &effectParams, &inBuffer, &outBuffer);

			iplAudioBufferInterleave(context, &outBuffer, outputaudioframe.data());

			std::copy(std::begin(outputaudioframe), std::end(outputaudioframe),
	   std::back_inserter(outputaudio));

			inData[0] += audioSettings.frameSize;
		}

		iplAudioBufferFree(context, &outBuffer);
		iplBinauralEffectRelease(&effect);
		iplHRTFRelease(&hrtf);
		iplContextRelease(&context);

		save_output_audio("outputaudio.raw", outputaudio);

	*/
	// glfwSetKeyCallback(window, glfw_keyCallback);
	// glfwSetKeyCallback(window, Shadow::KeyboardInput::glfw_keyCallback);

	bgfx::renderFrame();

	bgfx::Init init;

	init.platformData.ndt = glfwGetX11Display();
	init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(shadowWindow.window);

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
	const bgfx::ViewId kClearView = SCENE_VIEW_ID;
	bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xFFFFFFFF, 1.0f, 0);
	bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

	// ImGui init
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	// Load DebugUI fonts
	//    io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("./caskaydia-cove-nerd-font-mono.ttf", 16.0f);
	io.FontGlobalScale = 1.3f;

	ImGui::SetupTheme();

	ImGui_Implbgfx_Init(255);
	ImGui_ImplGlfw_InitForVulkan(shadowWindow.window, true);

	// Init stuffs

	const bgfx::Caps* rendererCapabilities = bgfx::getCaps();

	Shadow::Camera camera;
	camera.distance(10.0f);

	Shadow::FontManager fontManager;
	fontManager.createFontFromTTF();

	// Mesh* m_mesh = meshLoad("suzanne.bin");
	Shadow::Mesh mesh("suzanne.bin");

	Shadow::UserCode::loadUserCode();

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

	bgfx::VertexLayout pcvDecl;
	pcvDecl.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();
	bgfx::VertexBufferHandle vbh
		= bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), pcvDecl);
	bgfx::IndexBufferHandle ibh
		= bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

	bgfx::ProgramHandle program = loadProgram("test/vs_test.vulkan", "test/fs_test.vulkan");

	bgfx::ProgramHandle meshProgram = loadProgram("mesh/vs_mesh.vulkan", "mesh/fs_mesh.vulkan");

	// unsigned int counter = 0;

	float speed = 0.37f;
	float time = 0.0f;

	double lastMouseX, lastMouseY;

	char sampleData[256] = "Hello, World!";

	while (!shadowWindow.shouldClose()) {
		glfwPollEvents();

		if (shadowWindow.wasWindowResized()) {
			auto bounds = shadowWindow.getExtent();
			width = bounds.width;
			height = bounds.height;
			bgfx::reset(
				(uint32_t)width, (uint32_t)height, vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
			bgfx::setViewRect(EDITOR_BG_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);
		}

		Shadow::UserInput::updateMouse();

		// TODO: ImGui should probably be disabled for production use
		// #ifndef SHADOW_DEBUG_BUILD

		// ImGui
		ImGui_Implbgfx_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		// const ImGuiViewport* imguiViewport = ImGui::GetMainViewport();
		// ImGui::SetNextWindowPos(imguiViewport->WorkPos);
		// ImGui::SetNextWindowSize(imguiViewport->WorkSize);
		// ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		// ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		// ImGui::Begin("Dock space", nullptr,
		// 	ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground
		// 		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		// 		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);

		// ImGui::End();
		// ImGui::PopStyleVar(2);

		static MemoryEditor memedit;
		memedit.DrawWindow("Memory Editor", &sampleData, sizeof(sampleData));

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("Viewport", nullptr,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground
				| ImGuiWindowFlags_NoMouseInputs);
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

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
		ImGui::Text("MOUSE X: %i Y: %i", (int)UserInput::getMouseX(), (int)UserInput::getMouseY());
		ImGui::Text("Mouse Diff X: %i Y: %i", (int)UserInput::getMouseXDiff(),
			(int)UserInput::getMouseYDiff());
		ImGui::Text("Window Extents: W: %i, H: %i", width, height);
		ImGui::SliderFloat("FOV", &fov, 0.0f, 180.0f);
		ImGui::InputText("Name Demo", &nameDemo);
		ImGui::Checkbox("Vsync", &vsync);
		if (ImGui::Button("Close Window?"))
			shadowWindow.close();

		ImGui::Separator();
		ImGui::Text("Audio");
		if (ImGui::Button("Play Demo Audio"))
			ShadowAudio::playTestAudio();

		ImGui::Separator();
		ImGui::Text("UserCode");
		if (ImGui::Button("Reload UserCode Library"))
			Shadow::UserCode::loadUserCode();

		ImGui::End();

		ImGui::Render();

		ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

		// This dummy draw call is here to make sure that view 0 is cleared if no other draw calls
		// are submitted to view 0
		bgfx::touch(kClearView);
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

			bgfx::dbgTextPrintf(3, line++, 0xf0, "%s", sampleData);

#endif
		}

#ifdef SHADOW_DEBUG_BUILD
		const bgfx::Stats* stats = bgfx::getStats();
		bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
#endif

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
		const float aspectRatio = float(width) / float(height);
		bx::mtxProj(projectionMatrix, fov, aspectRatio, 0.01f, 1000.0f,
			rendererCapabilities->homogeneousDepth);

		bgfx::setViewTransform(SCENE_VIEW_ID, viewMatrix, projectionMatrix);
		bgfx::setViewRect(
			SCENE_VIEW_ID, vMin.x, vMin.y, uint16_t(vMax.x - vMin.x), uint16_t(vMax.y - vMin.y));

		bgfx::touch(SCENE_VIEW_ID);

		float orientation[16];
		bx::mtxIdentity(orientation);
		bgfx::setTransform(orientation);

		if (Shadow::UserInput::isLeftMouseDown() && !ImGui::MouseOverArea())
			camera.orbit(UserInput::getMouseXDiff() / 1000, UserInput::getMouseYDiff() / 1000);

		if (Shadow::UserInput::isRightMouseDown() && !ImGui::MouseOverArea())
			camera.dolly(UserInput::getMouseYDiff() / 1000);

		// if (key_backwards_pressed)
		// 	camZ -= 0.3f;
		// if (key_forwards_pressed)
		// 	camZ += 0.3f;
		// if (key_left_pressed)
		// 	camX += 0.3f;
		// if (key_right_pressed)
		// 	camX -= 0.3f;
		// if (key_up_pressed)
		// 	camZ += 0.3f;
		// if (key_down_pressed)
		// 	camZ -= 0.3f;

		bgfx::setVertexBuffer(0, vbh);
		bgfx::setIndexBuffer(ibh);

		float meshMtx[16];
		// mesh.submit(SCENE_VIEW_ID, meshProgram, meshMtx);

		bgfx::submit(SCENE_VIEW_ID, meshProgram);

		bgfx::frame();

		// counter++;
	}

	userSettingsDB.write("FOV", std::to_string(fov));
	userSettingsDB.write("Name", nameDemo);

	bgfx::destroy(ibh);
	bgfx::destroy(vbh);
	bgfx::destroy(program);
	bgfx::destroy(meshProgram);
	bgfx::destroy(u_time);

	mesh.unload();

	ImGui_ImplGlfw_Shutdown();
	ImGui_Implbgfx_Shutdown();

	// Shadow::fs::closeDB(userSettingsDB);

	ShutdownBXFilesytem();

	Shadow::ShutdownRuntime();

	return 0;
}

void Shadow::ShutdownRuntime() {
	// * Most of the classes shut themselves down at
	// * this point, which happens after this step
	ShadowAudio::shutdownAudioEngine();
	ImGui::DestroyContext();
	bgfx::shutdown();

	PRINT("Goodbye from Shadow Engine");
}
