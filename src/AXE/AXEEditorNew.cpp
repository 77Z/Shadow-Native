#include "AXEClipBrowser.hpp"
#include "AXENodeEditor.hpp"
#include "AXETimeline.hpp"
#include "Debug/Logger.hpp"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "AXE/AXEEditor.hpp"
#include "imgui/theme.hpp"
#include "ShadowWindow.hpp"
#include "Debug/EditorConsole.hpp"
#include "AXETypes.hpp"
#include "AXESerializer.hpp"
#include "ImGuiNotify.hpp"
#include "Keyboard.hpp"
#include "miniaudio.h"
#include "IconsCodicons.h"
#include "../implot/implot.h"

// Forward declarations
namespace Shadow::Util {
void openURL(const std::string &url);
}
namespace Shadow::AXE {
void updateHelpWindow(bool& p_open);
void bootstrapSong(const Song* song, ma_engine* audioEngine);
}

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

namespace Shadow::AXE {

const char* keysPretty[Keys_Count] = {
	"C",
	"C# / D♭",
	"D",
	"D# / E♭",
	"E",
	"F",
	"F# / G♭",
	"G",
	"G# / A♭",
	"A",
	"A# / B♭",
	"B"
};

static Song songInfo;

// Not persistent
static EditorState editorState;

static ClipBrowser* gblClipBrowser;

int startAXEEditor(std::string projectFile) {

	ma_result result;
	ma_engine engine;
	result = ma_engine_init(nullptr, &engine);
	if (result != MA_SUCCESS) {
		ERROUT("Failed to initialize ShadowAudio engine!!");
		return 1;
	}

	deserializeSong(&songInfo, projectFile);
	bootstrapSong(&songInfo, &engine);

	// ma_device* device = ma_engine_get_device(&engine);
	ma_sound sound;
	result = ma_sound_init_from_file(&engine, "./Resources/sound.wav", songInfo.decodeOnLoad ? MA_SOUND_FLAG_DECODE : 0, nullptr, nullptr, &sound);

	ma_uint64 length;
	ma_data_source_get_length_in_pcm_frames(&sound, &length);

	ma_context context;
	ma_context_init(nullptr, 0, nullptr, &context);
	ma_device_info* pPlaybackDeviceInfos;
    ma_uint32 playbackDeviceCount;
    ma_device_info* pCaptureDeviceInfos;
    ma_uint32 captureDeviceCount;
    ma_uint32 iDevice;
	ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
	for (iDevice = 0; iDevice < playbackDeviceCount; iDevice++)
		EC_WARN("All", "   %u: %s\n", iDevice, pCaptureDeviceInfos[iDevice].name);

	uint32_t sampleRate = ma_engine_get_sample_rate(&engine);


#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	ShadowWindow window(1280, 720, "Shadow Engine - AXE Editor", true, true);
	if (window.window == nullptr) return 1;
	glfwMakeContextCurrent(window.window);
	glfwSwapInterval(1); // Enable vsync

	Keyboard keyboard(&window);

	// Keyboard Shortcuts
	keyboard.registerKeyCallback([&](KeyButton_ key, bool down, KeyModifiers_ mods) {
		if (mods == KeyModifiers_Control
			&& key == KeyButton_S
			&& down) {
				serializeSong(&songInfo, projectFile);
				ImGui::InsertNotification({ImGuiToastType::Info, 3000, "Project Saved"});
			}

		if (mods == KeyModifiers_Control
			&& key == KeyButton_L
			&& down) {
				songInfo.tracks.clear();
				deserializeSong(&songInfo, projectFile);
				bootstrapSong(&songInfo, &engine);
				ImGui::InsertNotification({ImGuiToastType::Info, 3000, "Loaded project from computer"});
			}

		// TODO: This don't work
		if (mods == (KeyModifiers_Control | KeyModifiers_Shift)
			&& key == KeyButton_Q
			&& down) {
				ImGui::InsertNotification({ImGuiToastType::Success, 3000, "Bruh"});
			}
	});

	IMGUI_CHECKVERSION();
	ImPlot::CreateContext();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	ImGui::SetupTheme();

	ImGui_ImplGlfw_InitForOpenGL(window.window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Font loading and scaling
	{
		// IMGUI_ENABLE_FREETYPE in imconfig to use Freetype for higher quality font rendering
		editorState.sf = window.getContentScale();
		float sf = editorState.sf;

		float fontSize = 16.0f * sf;
		float iconFontSize = (fontSize * 2.0f / 3.0f) * sf;

		ImVector<ImWchar> ranges;
		ImFontGlyphRangesBuilder builder;
		builder.AddText("♭");

		ImFontConfig fontCfg;
		fontCfg.OversampleH = 4;
		fontCfg.OversampleV = 4;
		fontCfg.PixelSnapH = false;

		// ImFont* primaryFont = io.Fonts->AddFontFromFileTTF("./Resources/caskaydia-cove-nerd-font-mono.ttf", fontSize, &fontCfg, ranges.Data);
		ImFont* primaryFont = io.Fonts->AddFontFromFileTTF("./Resources/arial.ttf", fontSize, &fontCfg, ranges.Data);

		static const ImWchar iconRanges[] = { ICON_MIN_CI, ICON_MAX_CI, 0 };

		ImFontConfig iconFontCfg;
		iconFontCfg.GlyphMinAdvanceX = iconFontSize;
		iconFontCfg.MergeMode = true;
		iconFontCfg.PixelSnapH = true;
		iconFontCfg.OversampleH = 2;
		iconFontCfg.OversampleV = 2;
		iconFontCfg.GlyphOffset.y = 6;
		iconFontCfg.DstFont = primaryFont;

		io.Fonts->AddFontFromFileTTF("./Resources/codicon.ttf", 20.0f * sf, &iconFontCfg, iconRanges);

		ImGui::GetStyle().ScaleAllSizes(sf);
	}
	
	Timeline timeline(&songInfo, &editorState, &engine);
	ClipBrowser clipBrowser(&engine);
	AXENodeEditor nodeEditor;

	gblClipBrowser = &clipBrowser;
	glfwSetDropCallback(window.window, [](GLFWwindow* window, int count, const char** paths) {
		for (int i = 0; i < count; i++) {
			EC_PRINT("All", "User dropped: %s", paths[i]);
			gblClipBrowser->addFileToLibrary(paths[i]);
		}
	});

	window.maximize();

	while (!window.shouldClose()) {
		window.pollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Header and Menubar
		{
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
				| ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 255));

			ImGui::Begin("RootWindow", nullptr, window_flags);

			ImGui::PopStyleColor();
			ImGui::PopStyleVar(3);

			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("Save Project", "CTRL + S")) {
						serializeSong(&songInfo, projectFile);
						ImGui::InsertNotification({ImGuiToastType::Info, 3000, "Project Saved"});
					}
					ImGui::Separator();
					ImGui::CheckboxFlags("Drag windows out", &ImGui::GetIO().ConfigFlags, ImGuiConfigFlags_ViewportsEnable);
					ImGui::Separator();
					if (ImGui::MenuItem("Exit")) window.close();
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit")) {
					ImGui::MenuItem("Copy", "CTRL + C");
					ImGui::MenuItem("Paste", "CTRL + V");
					ImGui::Separator();
					ImGui::MenuItem("Project Metadata", nullptr, &editorState.showProjectMetadata);
					ImGui::MenuItem("Node Editor", nullptr, &editorState.showNodeEditor);
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Debug Tools")) {
					ImGui::MenuItem("Shadow Engine Debug Console", nullptr, &editorState.showShadowEngineConsole);
					ImGui::MenuItem("UI Console", nullptr, &editorState.showImGuiConsole);
					ImGui::MenuItem("UI Metrics", nullptr, &editorState.showImGuiMetrics);
					ImGui::MenuItem("UI Stack Tool", nullptr, &editorState.showImGuiStackTool);
					ImGui::MenuItem("Clipboard Buffer", nullptr, &editorState.showClipboardBuffer);
					if (ImGui::MenuItem("Reload Song", "CTRL + L")) {
						songInfo.tracks.clear();
						deserializeSong(&songInfo, projectFile);
						bootstrapSong(&songInfo, &engine);
						ImGui::InsertNotification({ImGuiToastType::Info, 3000, "Loaded project from computer"});
					}
					ImGui::MenuItem("Node Editor Debugger", nullptr, &editorState.showNodeEditorDebugger);
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Help!")) {
					ImGui::MenuItem("ShadowAudio information and help", nullptr, &editorState.showHelpDocs);
					ImGui::Text("If you need help, just text me");
					if (ImGui::MenuItem("Shadow Engine Website")) Util::openURL("https://shadow.77z.dev");
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::PushItemWidth(100.0f * editorState.sf);
			ImGui::InputFloat("BPM", &songInfo.bpm, 1.0f, 5.0f);
			ImGui::PushItemWidth(120.0f * editorState.sf);
			ImGui::SameLine();
			ImGui::InputInt2("Time Signature", songInfo.timeSignature);
			ImGui::SameLine();

			const char* keyName = (songInfo.key >= 0 && songInfo.key < Keys_Count) ? keysPretty[songInfo.key] : "? Unknown ?";
			ImGui::SliderInt("Key", &songInfo.key, 0, Keys_Count - 1, keyName);

			ImGui::SameLine();
			ImGui::SliderFloat("Master Vol", &songInfo.masterVolume, 0.0f, 1.0f);

			ImGui::SameLine();
			if (ImGui::Button(ICON_CI_PLAY)) {
				// timeline.play();
			}

			ImGui::SetCursorPosY(55.0f * editorState.sf);
			ImGui::DockSpace(ImGui::GetID("AXEDockspace"));
			ImGui::End();
		}

		//TODO: change this to callback sliders
		if (songInfo.masterVolume != editorState.lastKnownMasterVol) {
			ma_engine_set_volume(&engine, songInfo.masterVolume);
			EC_PRINT("All", "Changed master volume to %.3f", songInfo.masterVolume);
			editorState.lastKnownMasterVol = songInfo.masterVolume;
		}

		//ShadowAudio window
		{
			ImGui::Begin("ShadowAudio");

			if (ImGui::Button("Play sound at 4s time")) {
				// ma_engine_play_sound(&engine, "./Resources/sound.wav", nullptr);
				ma_sound_seek_to_pcm_frame(&sound, 0);
				ma_sound_set_start_time_in_pcm_frames(&sound, sampleRate * 4);
				ma_sound_start(&sound);
			}

			ImGui::Text("Engine Sample rate: %u", sampleRate);
			ImGui::Text("Engine milis: %llu", ma_engine_get_time_in_milliseconds(&engine));
			ImGui::Text("Engine PCM Frames: %llu", ma_engine_get_time_in_pcm_frames(&engine));
			ImGui::Text("Sound starts at PCM frame: %u", sampleRate * 4);
			// ImGui::Text("loaded sound length in pcm frames: %llu", length);

			if (ImGui::Button("Set engine timer to 0")) {
				ma_engine_set_time_in_pcm_frames(&engine, 0);
			}

			if (ImGui::Button("Start engine")) {
				ma_engine_start(&engine);
			}

			if (ImGui::Button("Stop engine")) {
				ma_engine_stop(&engine);
			}

			ImGui::End();
		}

		// Project Metadata
		if (editorState.showProjectMetadata) {
			ImGui::Begin("Project Metadata", &editorState.showProjectMetadata, ImGuiWindowFlags_NoCollapse);

			ImGui::Text("Project file path:\n%s", projectFile.c_str());

			ImGui::InputText("Song name", &songInfo.name);
			ImGui::TextUnformatted("^^^ Doesn't affect project file name ^^^");
			ImGui::InputText("Artist", &songInfo.artist);
			ImGui::InputText("Album", &songInfo.album);

			ImGui::Checkbox("Decode on load", &songInfo.decodeOnLoad);
			if (ImGui::BeginItemTooltip()) {
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(
					"This setting controls how audio data is loaded into memory. When left "
					"unchecked, audio data is loaded directly from the filesystem bit-for-bit into "
					"memory, and is decoded on the fly when it is needed. This saves space in "
					"memory if you're running low or have a lot of audio to process, but in more "
					"complicated processing scenerios this can cause defects, delays, stutters, "
					"and other artifacts in your audio.\nChecking this box will tell ShadowAudio "
					"to decode the audio ahead of time, which takes up more memory, but lessens "
					"the strain on the audio thread during playback.\n!!! This requires you to "
					"restart AXE in order for the change to take effect !!!");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			ImGui::TextUnformatted("Remember CTRL + S to save your changes!!");

			ImGui::End();
		}

		if (editorState.showShadowEngineConsole) EditorConsole::Frontend::onUpdate();
		if (editorState.showImGuiConsole) ImGui::ShowDebugLogWindow(&editorState.showImGuiConsole);
		if (editorState.showImGuiMetrics) ImGui::ShowMetricsWindow(&editorState.showImGuiMetrics);
		if (editorState.showImGuiStackTool) ImGui::ShowStackToolWindow(&editorState.showImGuiStackTool);

		ImGui::ShowDemoWindow();
		ImPlot::ShowDemoWindow();
		updateHelpWindow(editorState.showHelpDocs);
		timeline.onUpdate();
		nodeEditor.onUpdate(editorState.showNodeEditor);
		nodeEditor.updateDebugMenu(editorState.showNodeEditorDebugger);
		clipBrowser.onUpdate(editorState.showClipBrowser);
		ImGui::RenderNotifications();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window.window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window.window);
	}

	nodeEditor.shutdown();
	clipBrowser.shutdown();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	ma_sound_uninit(&sound);
	ma_engine_uninit(&engine);

	window.shutdown();

	return 0;
}

}