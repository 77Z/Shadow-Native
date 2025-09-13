#include "AXEClipBrowser.hpp"
#include "AXENodeEditor.hpp"
#include "AXETimeline.hpp"
#include "Debug/Logger.hpp"
#include "bx/bx.h"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl.h>
#include <cmath>
#include <cstdint>
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
#include "ShadowIcons.hpp"
#include "../implot/implot.h"
#include "../imgui/imgui_knobs.hpp"
#include "AXEEqualizer.hpp"
#include "AXEJobSystem.hpp"
#include "AXEGlobalSettingsWindow.hpp"
#include "AXEPianoRoll.hpp"
#include "AXEDrumEngine.hpp"
#include "AxevstPlugins.hpp"

// Forward declarations
namespace Shadow::Util {
void openURL(const std::string &url);
}
namespace Shadow::AXE {
void updateHelpWindow(bool& p_open);
void bootstrapSong(Song* song, ma_engine* audioEngine);
void unloadSong(Song* song, ma_engine* audioEngine);
void updateCurveTest();
void initCurveTest();
void cacheWaveforms();
void updateIconDebugWindow(bool& p_open);
void drawScrubberClockWidget(Song& song, EditorState& edState, uint64_t playbackFrames);
}
namespace Shadow::AXE::Account {
void onUpdateStatusBar(bool isInEditor, ShadowWindow* window);
}
namespace bx {
void debugBreak();
}

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to
// maximize ease of testing and compatibility with old VS compilers. To link
// with VS2010-era libraries, VS2015+ requires linking with
// legacy_stdio_definitions.lib, which we do using this pragma. Your own project
// should not be affected, as you are likely to link with a newer binary of GLFW
// that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

namespace Shadow::AXE {

static Song songInfo;

// Not persistent
static EditorState editorState;

static ClipBrowser* gblClipBrowser;

int startAXEEditor(std::string projectFile) {

	initCurveTest();

	loadGlobalSettingsFromDisk();

	ma_result result;
	ma_engine_config engineConfig = ma_engine_config_init();
	engineConfig.noAutoStart = MA_TRUE;
	ma_engine engine;
	result = ma_engine_init(&engineConfig, &engine);
	if (result != MA_SUCCESS) {
		ERROUT("Failed to initialize ShadowAudio engine!!");
		return 1;
	}

	editorState.sampleRate = (uint32_t)ma_engine_get_sample_rate(&engine);

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
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	ShadowWindow window(1280, 720, "Shadow Engine - AXE Editor", true, true);
	if (window.window == nullptr) return 1;
	glfwMakeContextCurrent(window.window);
	glfwSwapInterval(1); // Enable vsync

	Keyboard keyboard(&window);

	// These things should be init'd after the song loads. Bad things happen otherwise :(
	AXEDrumEngine drumEngine(&songInfo, &editorState, &engine);
	PianoRoll pianoRoll(&songInfo, &editorState, &engine);
	AXENodeEditor nodeEditor(&songInfo, &editorState);
	Timeline timeline(&songInfo, &editorState, &engine, &window, &nodeEditor, &drumEngine, &pianoRoll);
	ClipBrowser clipBrowser;
	AXEEqualizer equalizer;
	AXEVSTPlugins vstPlugins;

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
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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
		builder.AddText((const char*)u8"♭");

		ImFontConfig fontCfg;
		fontCfg.OversampleH = 4;
		fontCfg.OversampleV = 4;
		fontCfg.PixelSnapH = false;

		ImFont* primaryFont = io.Fonts->AddFontFromFileTTF("./Resources/Inter-Medium.ttf", fontSize, &fontCfg, ranges.Data);
		// ImFont* primaryFont = io.Fonts->AddFontFromFileTTF("./Resources/arial.ttf", fontSize, &fontCfg, ranges.Data);

		static const ImWchar iconRanges[] = { SHADOW_ICON_MIN, SHADOW_ICON_MAX, 0 };

		ImFontConfig iconFontCfg;
		iconFontCfg.GlyphMinAdvanceX = iconFontSize;
		iconFontCfg.MergeMode = true;
		iconFontCfg.PixelSnapH = true;
		iconFontCfg.OversampleH = 2;
		iconFontCfg.OversampleV = 2;
		iconFontCfg.GlyphOffset.y = 6;
		iconFontCfg.DstFont = primaryFont;

		io.Fonts->AddFontFromFileTTF("./Resources/shadow.ttf", 20.0f * sf, &iconFontCfg, iconRanges);

		editorState.headerFont = io.Fonts->AddFontFromFileTTF("./Resources/Inter-Black.ttf", 40.0f * sf);
		editorState.segmentDisplayFont = io.Fonts->AddFontFromFileTTF("./Resources/DSEG14Classic-Regular.ttf", fontSize);

		ImGui::GetStyle().ScaleAllSizes(sf);
	}

	gblClipBrowser = &clipBrowser;
	glfwSetDropCallback(window.window, [](GLFWwindow* window, int count, const char** paths) {
		BX_UNUSED(window);
		for (int i = 0; i < count; i++) {
			EC_PRINT("All", "User dropped: %s", paths[i]);
			gblClipBrowser->addFileToLibrary(paths[i]);
		}
	});

	ImGui::InsertNotification({
		ImGuiToastType::Info,
		5000,
		"You might have to wait for active\njobs to finish before working"
	});

	// cacheWaveforms();

	window.maximize();

	const float scroll_multiplier = 0.3f;
	const float scroll_smoothing = 8.0f;
	static ImVec2 scroll_energy = ImVec2(0.0f, 0.0f);

	while (!window.shouldClose()) {
		ImGui_ImplGlfw_WaitForEvent();
		window.pollEvents();
#if 0
		ImVec2 scrollData = ImGui_VT_getLatestScrollData();
		scrollData.x *= scroll_multiplier;
		scrollData.y *= scroll_multiplier;
		// Immediately stop if direction changes
		if(scroll_energy.x * scrollData.x < 0.0f) {
			scroll_energy.x = 0.0f;
		}
		if(scroll_energy.y * scrollData.y < 0.0f) {
			scroll_energy.y = 0.0f;
		}
		scroll_energy.x += scrollData.x;
		scroll_energy.y += scrollData.y;

		// Part 2 smooth scroll

		ImVec2 scroll_now = ImVec2(0.0f, 0.0f);
		// if(std::abs(scroll_energy.x) > 0.01f) {
			scroll_now.x = scroll_energy.x * io.DeltaTime * scroll_smoothing;
			scroll_energy.x -= scroll_now.x;
		// } else {
			// Cutoff smoothing when it's basically stopped
			// scroll_energy.x = 0.0f;
		// }
		if(std::abs(scroll_energy.y) > 0.1f) {
			scroll_now.y = scroll_energy.y * io.DeltaTime * scroll_smoothing;
			scroll_energy.y -= scroll_now.y;
		} else {
			// Cutoff smoothing when it's basically stopped
			scroll_energy.y = 0.0f;
		}
		io.MouseWheel = scroll_now.y;
		io.MouseWheelH = -scroll_now.x;
#endif

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Header and Menubar
		{
			using namespace ImGui;

			ImGuiViewport* viewport = GetMainViewport();
			SetNextWindowPos(viewport->Pos);
			SetNextWindowSize(viewport->Size);
			SetNextWindowViewport(viewport->ID);
			PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
				| ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 255));

			Begin("RootWindow", nullptr, window_flags);

			PopStyleColor();
			PopStyleVar(3);

			if (BeginMenuBar()) {
				if (BeginMenu("File")) {
					if (MenuItem("Save Project", "CTRL + S")) {
						serializeSong(&songInfo, projectFile);
						InsertNotification({ImGuiToastType::Info, 3000, "Project Saved"});
					}
					Separator();
					CheckboxFlags("Drag windows out", &GetIO().ConfigFlags, ImGuiConfigFlags_ViewportsEnable);
					Separator();
					if (MenuItem("Exit")) window.close();
					//TODO: Fix me!!
					ImGui::EndMenu();
				}
				if (BeginMenu("Edit")) {
					MenuItem("Copy", "CTRL + C");
					MenuItem("Paste", "CTRL + V");
					Separator();
					MenuItem("Project Metadata", nullptr, &editorState.showProjectMetadata);
					MenuItem("Node Editor", nullptr, &editorState.showNodeEditor);
					MenuItem("Visual Equalizer", nullptr, &editorState.showEqualizer);
					MenuItem("VST Plugins", nullptr, &editorState.showVSTWindow);
					Separator();
					MenuItem("AXE Global Settings", "CTRL + ,", &editorState.showGlobalSettings);
					ImGui::EndMenu();
				}
				if (BeginMenu("View")) {
					SeparatorText("Timeline Units");
					RadioButton("Beats", (int*)&songInfo.timelineUnits, TimelineUnit_BPM);
					RadioButton("Seconds", (int*)&songInfo.timelineUnits, TimelineUnit_TimeScale);
					RadioButton("PCM Frames", (int*)&songInfo.timelineUnits, TimelineUnit_PCMFrames);
					SetItemTooltip("Useful for Vince debugging");
					SeparatorText("Editor Theme");
					if (MenuItem("Default Theme")) SetupTheme();
					if (MenuItem("Sad Theme")) SetupSadTheme();
					ImGui::EndMenu();
				}
				if (BeginMenu("Debug Tools")) {
					MenuItem("Shadow Engine Debug Console", nullptr, &editorState.showShadowEngineConsole);
					MenuItem("UI Console", nullptr, &editorState.showImGuiConsole);
					MenuItem("UI Metrics", nullptr, &editorState.showImGuiMetrics);
					MenuItem("UI Stack Tool", nullptr, &editorState.showImGuiStackTool);
					MenuItem("Clipboard Buffer", nullptr, &editorState.showClipboardBuffer);
					if (MenuItem("Reload Song", "CTRL + L")) {
						songInfo.tracks.clear();
						deserializeSong(&songInfo, projectFile);
						bootstrapSong(&songInfo, &engine);
						InsertNotification({ImGuiToastType::Info, 3000, "Loaded project from computer"});
					}
					MenuItem("Node Editor Debugger", nullptr, &editorState.showNodeEditorDebugger);
					MenuItem("Automation Debug Mode", nullptr, &editorState.automationDebugMode);
					MenuItem("Timeline Cursor Position Debug Mode", nullptr, &editorState.timelinePositionDebugMode);
					MenuItem("Bookmark Debugger", nullptr, &editorState.showBookmarksDebugger);
					MenuItem("Icon Table", nullptr, &editorState.showIconDebugger);
					if (MenuItem("Re-cache waveforms")) cacheWaveforms();
					Separator();
					if (MenuItem("Break Here")) {
						bx::debugBreak();
					}
					if (BeginItemTooltip()) {
						PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
						if (fmodf((float)GetTime(), 0.4f) < 0.2f) {
							TextUnformatted("WARNING: WITHOUT A DEBUGGER ATTACHED, THIS WILL CRASH AXE!!!");
						}
						PopStyleColor();
						EndTooltip();
					}
					ImGui::EndMenu();
				}
				if (BeginMenu("Help!")) {
					if (MenuItem("Diagnose issues!")) OpenPopup("Dump and Ship");
					Separator();
					MenuItem("ShadowAudio information and help", nullptr, &editorState.showHelpDocs);
					Text("If you need help, just text me");
					if (MenuItem("Shadow Engine Website")) Util::openURL("https://shadow.77z.dev");
					ImGui::EndMenu();
				}
				EndMenuBar();
			}

			PushItemWidth(100.0f * editorState.sf);
			InputFloat("BPM", &songInfo.bpm, 1.0f, 5.0f);
			PushItemWidth(120.0f * editorState.sf);
			SameLine();
			// InputInt2("Time Signature", songInfo.timeSignature);
			// SameLine();

			#if 0
			const char* keyName = (songInfo.key >= 0 && songInfo.key < Keys_Count) ? keysPretty[songInfo.key] : "? Unknown ?";
			// SliderInt("Key", &songInfo.key, 0, Keys_Count - 1, keyName);
			if (BeginCombo("Key", keyName)) {
				for (int n = 0; n < ((int)(sizeof(keysPretty) / sizeof(*(keysPretty)))); n++) {
					const bool isSelected = (songInfo.key == n);
					if (Selectable(keysPretty[n], isSelected))
						songInfo.key = n;

					if (isSelected) SetItemDefaultFocus();
				}
				EndCombo();
			}
			#endif

			DragScalar("Song Len", ImGuiDataType_U64, &songInfo.songLength);

			SameLine();
			if (SliderFloat("Master Vol", &songInfo.masterVolume, 0.0f, 1.0f)) {
				ma_engine_set_volume(&engine, songInfo.masterVolume);
				EC_PRINT("All", "Changed master volume to %.3f", songInfo.masterVolume);
			}

			SameLine();
			SliderFloat("Zoom", &editorState.zoom, 10.0f, 600.0f, "%.0f%%");

			SameLine();
			if (Button(SHADOW_ICON_ZOOM_IN)) editorState.zoom = 100.0f;
			SetItemTooltip(SHADOW_ICON_ZOOM_IN " Reset zoom");

			SameLine();
			if (Button(SHADOW_ICON_DIFF_ADDED)) timeline.addTrack();
			SetItemTooltip("New Track");

			SameLine();
			ToggleButton(SHADOW_ICON_CLIP_CUT, &timeline.singleSlicingClip);
			SetItemTooltip("Slice clip...");

			SameLine();
			ToggleButton(SHADOW_ICON_MULTI_CLIP_CUT, &timeline.multiSlicingClip);
			SetItemTooltip("Multi slice clip cut");

			SameLine();
			if (Button(SHADOW_ICON_BOOKMARK)) timeline.newBookmark();
			SetItemTooltip(SHADOW_ICON_BOOKMARK " New bookmark (Ctrl + B)");

			SameLine();
			ToggleButton(SHADOW_ICON_MAGNET, &editorState.snappingEnabled);
			SetItemTooltip(SHADOW_ICON_MAGNET " Toggle clip snapping");

			SameLine();
			if (Button(timeline.isPlaying() ? SHADOW_ICON_DEBUG_PAUSE : SHADOW_ICON_PLAY)) {
				timeline.togglePlayback();
			}
			SetItemTooltip("Play/Pause song from current position (SPACE)");

			SameLine();
			JobSystem::onUpdateStatusBar();

			SameLine();
			drawScrubberClockWidget(songInfo, editorState, timeline.getPlaybackFrames());

			SameLine();
			SetCursorPos(ImVec2(GetWindowSize().x - 50, 25));
			Account::onUpdateStatusBar(true, &window);

			SetCursorPosY(55.0f * editorState.sf);
			DockSpace(GetID("AXEDockspace"));
			End();
		}

		if (ImGui::BeginPopupModal("Dump and Ship")) {

			ImGui::TextWrapped(
				"When you encounter problems in AXE, i'll probably tell you"
				" to press this button. It packages all debug and logging info"
				" about your AXE session and ships it off to my server.\n\n"
				"It probably won't be enough to fix every problem, but"
				" hopefully it can be helpful"
			);

			if (ImGui::Button("Send it")) {
				// TODO:
			}

			if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		//ShadowAudio window
		{
			using namespace ImGui;
			Begin("ShadowAudio");

			PushFont(editorState.headerFont);
			TextUnformatted("AXE Editor");
			PopFont();

			TextUnformatted(reinterpret_cast<const char*>(glGetString(GL_VERSION)));

			SeparatorText("Basic debugging info");

			ImGuiIO& io = GetIO();
			Text("UI FPS: %.0f (average frametime: %.3f ms/frame)", io.Framerate, 1000.0f / io.Framerate);
			Text("%d verts  :  %d indices  :  %d tris", io.MetricsRenderVertices, io.MetricsRenderIndices, io.MetricsRenderIndices / 3);
			Text("UI scaling %.3f", window.getContentScale());
			Text("Sample Rate: %u PCM/S", editorState.sampleRate);

			Separator();

			if (Button("Play sound at 4s time")) {
				// ma_engine_play_sound(&engine, "./Resources/sound.wav", nullptr);
				ma_sound_seek_to_pcm_frame(&sound, 0);
				ma_sound_set_start_time_in_pcm_frames(&sound, sampleRate * 4);
				ma_sound_start(&sound);
			}

			Text("Engine Sample rate: %u", sampleRate);
			Text("Engine milis: %llu", ma_engine_get_time_in_milliseconds(&engine));
			Text("Engine PCM Frames: %llu", ma_engine_get_time_in_pcm_frames(&engine));
			Text("Sound starts at PCM frame: %u", sampleRate * 4);
			// ImGui::Text("loaded sound length in pcm frames: %llu", length);

			if (Button("Set engine timer to 0")) {
				ma_engine_set_time_in_pcm_frames(&engine, 0);
			}

			if (Button("Start engine")) {
				ma_engine_start(&engine);
			}

			if (Button("Stop engine")) {
				ma_engine_stop(&engine);
			}

			ImGuiKnobs::Knob("Master Vol", &songInfo.masterVolume, 0.0f, 1.0f);

			DebugTextEncoding((const char*)u8"C# / D♭");

			if (Button("Big number modal"))
				OpenPopup("BigNumberInFile");

			if (BeginPopupModal("BigNumberInFile")) {

				TextWrapped("A an extremely large number has been detected in your project and the "
							"traditional AXE data format is not designed to handle numbers this "
							"big. There is a seperate format that is less space efficient");

				if (Button("Okay!"))	
					CloseCurrentPopup();

				EndPopup();
			}

			End();
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

		updateCurveTest();
		ImGui::ShowDemoWindow();
		// ImPlot::ShowDemoWindow();
		updateHelpWindow(editorState.showHelpDocs);
		timeline.onUpdate();
		nodeEditor.onUpdate(editorState.showNodeEditor);
		nodeEditor.updateDebugMenu(editorState.showNodeEditorDebugger);
		clipBrowser.onUpdate(editorState.showClipBrowser);
		equalizer.onUpdate(editorState.showEqualizer);
		pianoRoll.onUpdate();
		updateIconDebugWindow(editorState.showIconDebugger);
		drumEngine.onUpdate();
		timeline.updateBookmarkDebugMenu(editorState.showBookmarksDebugger);
		onUpdateGlobalSettingsWindow(editorState.showGlobalSettings);
		vstPlugins.onUpdate(editorState.showVSTWindow);
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
	unloadSong(&songInfo, &engine);

	window.shutdown();

	return 0;
}

}
