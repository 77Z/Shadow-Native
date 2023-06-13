#include "Components/Camera.h"
#include "types.h"
#include "shadow/audio.h"
#include "Runtime.h"
#include "UserCode.h"
#include "Logger.h"
#include <boost/algorithm/string/case_conv.hpp>
#include <bx/math.h>
#include <bx/string.h>
#include <bx/file.h>
#include <Util.h>
#include <boost/algorithm/string.hpp>
#include <generated/autoconf.h>

#include "shadow_fs.h"
#include <leveldb/db.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <imgui_node_editor.h>
#include <imgui_internal.h>

//#include <phonon.h>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <vector>

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
#include <imgui/imgui_impl_bgfx.h>

#include <unistd.h>

/*#ifdef SHADOW_DEBUG_BUILD
#define IMGUI_VULKAN_DEBUG_REPORT
#endif*/

namespace ed = ax::NodeEditor;

static ed::EditorContext* g_Context = nullptr;

static bool s_showStats = false;
static bool s_showWarningText = true;

static void glfw_errorCallback(int error, const char *description) {
	fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
		s_showWarningText = !s_showWarningText;

	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
		s_showStats = !s_showStats;
}

static const glm::vec2 SIZE = glm::vec2(1280, 720);

struct Vertex {
	glm::vec3 position;
	u32 color;
};

struct PosColorVertex
{
    float x;
    float y;
    float z;
    uint32_t abgr;
};

static PosColorVertex cubeVertices[] =
{
    {-1.0f,  1.0f,  1.0f, 0xff000000 },
    { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
    { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    {-1.0f,  1.0f, -1.0f, 0xffff0000 },
    { 1.0f,  1.0f, -1.0f, 0xff0000ff },
    {-1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeTriList[] =
{
    0, 1, 2,
    1, 3, 2,
    4, 6, 5,
    5, 6, 7,
    0, 2, 4,
    4, 2, 6,
    1, 5, 3,
    5, 7, 3,
    0, 4, 1,
    4, 5, 1,
    2, 3, 6,
    6, 3, 7,
};

std::vector<float> load_input_audio(const std::string filename) {
	std::ifstream file(filename.c_str(), std::ios::binary);

	file.seekg(0, std::ios::end);
	auto filesize = file.tellg();
	auto numsamples = static_cast<int>(filesize / sizeof(float));

	std::vector<float> inputaudio(numsamples);
	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(inputaudio.data()), filesize);

	return inputaudio;
}

void save_output_audio(const std::string filename, std::vector<float> outputaudio) {
	std::ofstream file(filename.c_str(), std::ios::binary);
	file.write(reinterpret_cast<char*>(outputaudio.data()), outputaudio.size() * sizeof(float));
}

int Shadow::StartRuntime() {

	InitBXFilesystem();

	int width = 1280;
	int height = 720;
	
	//int width = 1920;
	//int height = 1080;

	glfwSetErrorCallback(glfw_errorCallback);
	if (!glfwInit())
		return 1;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//GLFWwindow *window = glfwCreateWindow(width, height, "Shadow Engine", glfwGetPrimaryMonitor(), nullptr);
	GLFWwindow *window = glfwCreateWindow(width, height, CONFIG_PRETTY_NAME, nullptr, nullptr);

	if (!window)
		return 1;

	IMGUI_CHECKVERSION();

	ShadowAudio::initAudioEngine();


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

		std::copy(std::begin(outputaudioframe), std::end(outputaudioframe), std::back_inserter(outputaudio));

		inData[0] += audioSettings.frameSize;
	}

	iplAudioBufferFree(context, &outBuffer);
	iplBinauralEffectRelease(&effect);
	iplHRTFRelease(&hrtf);
	iplContextRelease(&context);

	save_output_audio("outputaudio.raw", outputaudio);

*/




	glfwSetKeyCallback(window, glfw_keyCallback);

	bgfx::renderFrame();

	bgfx::Init init;

	init.platformData.ndt = glfwGetX11Display();
	init.platformData.nwh = (void*)(uintptr_t) glfwGetX11Window(window);

	glfwGetWindowSize(window, &width, &height);
	init.resolution.width = (uint32_t) width;
	init.resolution.height = (uint32_t) height;
	init.resolution.reset = BGFX_RESET_VSYNC;
	if (!bgfx::init(init))
		return 1;

	// Set view 0 to be the same dimensions as the window and to clear the color buffer
	const bgfx::ViewId kClearView = 0;
	bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
	bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

	// ImGui init
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
//    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	// Load DebugUI fonts
//    io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("./caskaydia-cove-nerd-font-mono.ttf", 16.0f);

	ImGui_Implbgfx_Init(255);
	ImGui_ImplGlfw_InitForVulkan(window, true);

	g_Context = ed::CreateEditor();

	// Init stuffs

	Shadow::Camera camera;

	//Mesh* m_mesh = meshLoad("desk.bin");

	Shadow::UserCode::loadUserCode(CONFIG_DYNAMIC_USERCODE_LIBRARY_LOCATION);

	// User Settings
	leveldb::DB* userSettingsDB = Shadow::fs::openDB("./us");

	userSettingsDB->Put(leveldb::WriteOptions(), "mykey", "Hello!");

	fs::writeDB(userSettingsDB, "myotherkey", "Hello again");

	int64_t m_timeOffset;
	bgfx::UniformHandle u_time = bgfx::createUniform("u_time", bgfx::UniformType::Vec4);

	bgfx::VertexLayout pcvDecl;
	pcvDecl.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();
	bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), pcvDecl);
	bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

	//bgfx::ShaderHandle vsh = loadShader("vs_test.vulkan");
	//bgfx::ShaderHandle fsh = loadShader("fs_test.vulkan");

	bgfx::ProgramHandle program = loadProgram("vs_test.vulkan", "fs_test.vulkan");

	unsigned int counter = 0;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// Handle Window Resize
		int oldWidth = width;
		int oldHeight = height;
		glfwGetWindowSize(window, &width, &height);
		if (width != oldWidth || height != oldHeight) {
			bgfx::reset((uint32_t) width, (uint32_t) height, BGFX_RESET_VSYNC);
			bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
		}

		//TODO: ImGui should probably be disabled for production use
		//#ifndef SHADOW_DEBUG_BUILD

		// ImGui
		ImGui_Implbgfx_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		ImGui::Begin("Shadow Engine");

			ImGui::Checkbox("Show Stats (F3)", &s_showStats);
			ImGui::Checkbox("Show warning text (F1)", &s_showWarningText);

			ImGui::Separator();
			ImGui::Text("Audio");
			if (ImGui::Button("Play Demo Audio"))
				ShadowAudio::playTestAudio();

			ImGui::Separator();
			ImGui::Text("UserCode");
			if (ImGui::Button("Reload UserCode Library"))
				Shadow::UserCode::loadUserCode(CONFIG_DYNAMIC_USERCODE_LIBRARY_LOCATION);

		ImGui::End();

		ImGui::Begin("Shader Editor");

			ed::SetCurrentEditor(g_Context);
			ed::Begin("My Editor");

				int uniqueId = 1;

				ed::BeginNode(uniqueId++);
					ImGui::Text("This is node %i", uniqueId);
					ed::BeginPin(uniqueId++, ed::PinKind::Input);
						ImGui::Text("In");
					ed::EndPin();
					ImGui::SameLine();
					ed::BeginPin(uniqueId++, ed::PinKind::Output);
						ImGui::Text("Out");
					ed::EndPin();
				ed::EndNode();

				ed::BeginNode(uniqueId++);
					ImGui::Text("Blah %i", uniqueId);
				ed::EndNode();

			ed::End();
		ImGui::End();

		ImGui::Render();

		ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

		// This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0
		bgfx::touch(kClearView);
		// Use debug font to print information about this example.
		bgfx::dbgTextClear();

		if (s_showWarningText) {
			bgfx::dbgTextPrintf(3, 2, 0x01, "DEBUG BUILD OF %s", boost::to_upper_copy<std::string>(CONFIG_PRETTY_NAME).c_str());
			bgfx::dbgTextPrintf(3, 3, 0x01, "NOT READY FOR PRODUCTION");
		}

#ifdef SHADOW_DEBUG_BUILD
		const bgfx::Stats *stats = bgfx::getStats();
		bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
#endif

		const bx::Vec3 at = {0.0f, 0.0f,  0.0f};
		const bx::Vec3 eye = {0.0f, 0.0f, -10.0f};
		float view[16];
		bx::mtxLookAt(view, eye, at);
		float proj[16];
		bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
		bgfx::setViewTransform(0, view, proj);

		float mtx[16];
		bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
		bgfx::setTransform(mtx);

		bgfx::setVertexBuffer(0, vbh);
		bgfx::setIndexBuffer(ibh);

		bgfx::submit(0, program);

		bgfx::frame();

		counter++;
	}

	bgfx::destroy(ibh);
	bgfx::destroy(vbh);
	bgfx::destroy(program);
	bgfx::destroy(u_time);

	ImGui_ImplGlfw_Shutdown();

	Shadow::fs::closeDB(userSettingsDB);

	ShutdownBXFilesytem();

	Shadow::ShutdownRuntime();

	return 0;
}

void Shadow::ShutdownRuntime() {
	ShadowAudio::shutdownAudioEngine();
	ed::DestroyEditor(g_Context);
	ImGui::DestroyContext();
	bgfx::shutdown();
	glfwTerminate();

	PRINT("Goodbye from Shadow Engine");
}
