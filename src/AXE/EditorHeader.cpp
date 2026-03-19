//
// Created by vince on 2/12/26.
//

#include "EditorHeader.hpp"
#include "AXEJobSystem.hpp"
#include "AXEOpenGLUtils.hpp"
#include "AXESerializer.hpp"
#include "AXETimeline.hpp"
#include "AXETypes.hpp"
#include "Configuration/EngineConfiguration.hpp"
#include "Debug/EditorConsole.hpp"
#include "ImGuiNotify.hpp"
#include "Util.hpp"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "imgui_internal.h"

// Forward Declarations
namespace Shadow::AXE {
void drawScrubberClockWidget(Song& song, EditorState& edState, uint64_t playbackFrames);
}
namespace Shadow::AXE::Account {
void onUpdateStatusBar(bool isInEditor, ShadowWindow* window);
}
namespace bx {
void debugBreak();
}

namespace Shadow::AXE {

static int imgWidth = 0, imgHeight = 0;
static GLuint windowDecorationImage;

void headerBarLoad() {
	windowDecorationImage = OpenGLLoadTextureFromFile("./Resources/AXEheader.png", &imgWidth, &imgHeight);
}


void drawHeaderBar(
	Song* songInfo,
	EditorState* editorState,
	ShadowWindow* window,
	Timeline* timeline,
	ma_engine engine,
	const std::string& projectFile
) {
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
		| /*ImGuiWindowFlags_MenuBar |*/ ImGuiWindowFlags_NoDocking;
	PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 255));

	Begin("RootWindow", nullptr, window_flags);

	PopStyleColor();
	PopStyleVar(3);

	{ // Backdrop shadow. Cool effect like in JetBrains editors
		SetCursorPos(ImVec2(0, 0));
		Image(reinterpret_cast<void*>(static_cast<intptr_t>(windowDecorationImage)),
			ImVec2((float)imgWidth * 1.0_scaled/* * editorState.sf*/, (float)imgHeight * 1.0_scaled/* * editorState.sf*/));


#if 0 // This would be the ideal way to do this, but we don't have the shadow extension for ImGui, so we can't...
		const auto diameter = 800.0f * editorState.sf;
		const auto pos = GetWindowPos() - ImVec2(0, diameter / 2);
		ImGui::GetWindowDrawList()->AddShadowCircle(pos, diameter / 2, ImGui::GetColorU32(ImGuiCol_ButtonActive, 0.8F), diameter / 4, ImVec2());
#endif
	}


	auto imguiRootWindow = GetCurrentWindow();
	BeginGroup();
	imguiRootWindow->DC.LayoutType = ImGuiLayoutType_Horizontal;
	imguiRootWindow->DC.CursorPos = imguiRootWindow->DC.CursorMaxPos = ImVec2(
		imguiRootWindow->DC.MenuBarOffset.x, imguiRootWindow->DC.MenuBarOffset.y + 30.0f);
	imguiRootWindow->DC.IsSameLine = false;
	imguiRootWindow->DC.NavLayerCurrent = ImGuiNavLayer_Menu;
	imguiRootWindow->DC.MenuBarAppending = true;
	AlignTextToFramePadding();
	// if (myBeginMenuBar()) {
	// if (BeginMenuBar()) {
		SetCursorPos(ImVec2(100.0_scaled, 6.0_scaled));
		if (BeginMenu("File")) {
			if (MenuItem("Save Project", "CTRL + S")) {
				serializeSong(songInfo, projectFile);
				InsertNotification({ImGuiToastType::Info, 3000, "Project Saved"});
			}
			Separator();
			CheckboxFlags("Drag windows out", &GetIO().ConfigFlags, ImGuiConfigFlags_ViewportsEnable);
			Separator();
			if (MenuItem("Exit")) window->close();
			//TODO: Fix me!!
			ImGui::EndMenu();
		}
		if (BeginMenu("Edit")) {
			MenuItem("Copy", "CTRL + C");
			MenuItem("Paste", "CTRL + V");
			Separator();
			MenuItem("Project Metadata", nullptr, &editorState->showProjectMetadata);
			MenuItem("Node Editor", nullptr, &editorState->showNodeEditor);
			MenuItem("Visual Equalizer", nullptr, &editorState->showEqualizer);
			MenuItem("VST Plugins", nullptr, &editorState->showVSTWindow);
			Separator();
			MenuItem("AXE Global Settings", "CTRL + ,", &editorState->showGlobalSettings);
			ImGui::EndMenu();
		}
		if (BeginMenu("View")) {
			SeparatorText("Timeline Units");
			RadioButton("Beats", (int*)&songInfo->timelineUnits, TimelineUnit_BPM);
			RadioButton("Seconds", (int*)&songInfo->timelineUnits, TimelineUnit_TimeScale);
			RadioButton("PCM Frames", (int*)&songInfo->timelineUnits, TimelineUnit_PCMFrames);
			SetItemTooltip("Useful for Vince debugging");
#if 0
			SeparatorText("Editor Theme");
			if (MenuItem("Default Theme")) SetupTheme();
			if (MenuItem("Sad Theme")) SetupSadTheme();
#endif
			ImGui::EndMenu();
		}
		if (BeginMenu("Debug Tools")) {
			MenuItem("Shadow Engine Debug Console", nullptr, &editorState->showShadowEngineConsole);
			MenuItem("UI Console", nullptr, &editorState->showImGuiConsole);
			MenuItem("UI Metrics", nullptr, &editorState->showImGuiMetrics);
			MenuItem("UI Stack Tool", nullptr, &editorState->showImGuiStackTool);
			MenuItem("UI Style Editor", nullptr, &editorState->showImGuiStyleEditor);
			MenuItem("Clipboard Buffer", nullptr, &editorState->showClipboardBuffer);
			if (MenuItem("Reload Song", "CTRL + L")) {
#if 0
				songInfo->tracks.clear();
				deserializeSong(&songInfo, projectFile);
				bootstrapSong(&songInfo, &engine);
				InsertNotification({ImGuiToastType::Info, 3000, "Loaded project from computer"});
#endif
			}
			MenuItem("Node Editor Debugger", nullptr, &editorState->showNodeEditorDebugger);
			MenuItem("Automation Debug Mode", nullptr, &editorState->automationDebugMode);
			MenuItem("Timeline Cursor Position Debug Mode", nullptr, &editorState->timelinePositionDebugMode);
			MenuItem("Timeline Debug Menu", nullptr, &editorState->showTimelineDebugger);
			MenuItem("Icon Table", nullptr, &editorState->showIconDebugger);
			if (MenuItem("Re-cache waveforms")) { }
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
			MenuItem("ShadowAudio information and help", nullptr, &editorState->showHelpDocs);
			Text("If you need help, just text me");
			if (MenuItem("Shadow Engine Website")) Util::openURL("https://shadow.77z.dev");
			ImGui::EndMenu();
		}
		// EndMenuBar();
	// }
	EndGroup();
	imguiRootWindow->DC.LayoutType = ImGuiLayoutType_Vertical;
	imguiRootWindow->DC.IsSameLine = false;
	imguiRootWindow->DC.NavLayerCurrent = ImGuiNavLayer_Main;
	imguiRootWindow->DC.MenuBarAppending = false;


	SetCursorPos(ImVec2(100.0_scaled, 35.0_scaled));

	PushItemWidth(100.0_scaled);
	InputFloat("BPM", &songInfo->bpm, 1.0f, 5.0f);
	PushItemWidth(120.0f * editorState->sf);
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

	DragScalar("Song Len", ImGuiDataType_U64, &songInfo->songLength);

	SameLine();
	if (SliderFloat("Master Vol", &songInfo->masterVolume, 0.0f, 1.0f)) {
		ma_engine_set_volume(&engine, songInfo->masterVolume);
		EC_PRINT("All", "Changed master volume to %.3f", songInfo->masterVolume);
	}

	SameLine();
	SliderFloat("Zoom", &editorState->zoom, 10.0f, 600.0f, "%.0f%%");

	SameLine();
	if (Button(SHADOW_ICON_ZOOM_IN)) editorState->zoom = 100.0f;
	SetItemTooltip(SHADOW_ICON_ZOOM_IN " Reset zoom");

	SameLine();
	if (Button(SHADOW_ICON_DIFF_ADDED)) timeline->addTrack();
	SetItemTooltip("New Track");

	SameLine();
	ToggleButton(SHADOW_ICON_CLIP_CUT, &timeline->singleSlicingClip);
	SetItemTooltip("Slice clip...");

	SameLine();
	ToggleButton(SHADOW_ICON_MULTI_CLIP_CUT, &timeline->multiSlicingClip);
	SetItemTooltip("Multi slice clip cut");

	SameLine();
	if (Button(SHADOW_ICON_BOOKMARK)) timeline->newBookmark();
	SetItemTooltip(SHADOW_ICON_BOOKMARK " New bookmark (Ctrl + B)");

	SameLine();
	ToggleButton(SHADOW_ICON_MAGNET, &editorState->snappingEnabled);
	SetItemTooltip(SHADOW_ICON_MAGNET " Toggle clip snapping");

	SameLine();
	if (Button(timeline->isPlaying() ? SHADOW_ICON_DEBUG_PAUSE : SHADOW_ICON_PLAY)) {
		timeline->togglePlayback();
	}
	SetItemTooltip("Play/Pause song from current position (SPACE)");

	SameLine();
	JobSystem::onUpdateStatusBar();

	SameLine();
	drawScrubberClockWidget(*songInfo, *editorState, timeline->getPlaybackFrames());


#if BX_PLATFORM_LINUX
	constexpr auto minimizeIcon = SHADOW_ICON_CHROME_MINIMIZE_LINUX;
	constexpr auto maximizeIcon = SHADOW_ICON_CHROME_MAXIMIZE_LINUX;
	constexpr auto restoreIcon  = SHADOW_ICON_CHROME_RESTORE_LINUX;
#else
	constexpr auto minimizeIcon = SHADOW_ICON_CHROME_MINIMIZE;
	constexpr auto maximizeIcon = SHADOW_ICON_CHROME_MAXIMIZE;
	constexpr auto restoreIcon  = SHADOW_ICON_CHROME_RESTORE;
#endif

	SetCursorPos(ImVec2(GetWindowSize().x - (55.0_scaled * 1), 0));
	if (WindowChromeButton(SHADOW_ICON_CHROME_CLOSE)) window->close();

	SetCursorPos(ImVec2(GetWindowSize().x - (55.0_scaled * 2), 0));
	if (glfwGetWindowAttrib(window->window, GLFW_MAXIMIZED)) {
		if (WindowChromeButton(restoreIcon)) glfwRestoreWindow(window->window);
	} else {
		if (WindowChromeButton(maximizeIcon)) glfwMaximizeWindow(window->window);
	}

	SetCursorPos(ImVec2(GetWindowSize().x - (55.0_scaled * 3), 0));
	if (WindowChromeButton(minimizeIcon)) glfwIconifyWindow(window->window);

	// 77Z Account button in top right
	SameLine();
	SetCursorPos(ImVec2(GetWindowSize().x - 200.0_scaled, 10.0_scaled));
	Account::onUpdateStatusBar(true, window);

	// Drag region?
	ImRect windowDragRegion = ImRect(
		imguiRootWindow->ViewportPos + ImVec2(370.0_scaled, 0.0f),
		imguiRootWindow->ViewportPos + ImVec2(GetWindowSize().x - 205.0_scaled, 30.0_scaled));
	// GetForegroundDrawList()->AddRect(windowDragRegion.Min, windowDragRegion.Max, IM_COL32(255, 255, 0, 255));

	if (windowDragRegion.Contains(GetMousePos()) && IsMouseClicked(ImGuiMouseButton_Left))
		window->dragWindow();

	{ // Window title
		const char* title = "AXE Audio Workstation";
		ImVec2 currentCursorPos = GetCursorPos();
		ImVec2 textSize = CalcTextSize(title);
		imguiRootWindow->DrawList->AddText(
			ImVec2(GetWindowPos().x + GetWindowWidth() * 0.5f - textSize.x * 0.5f, GetWindowPos().y + 8.0f),
			IM_COL32(255, 255, 255, 255), title);
		SetCursorPos(currentCursorPos);
	}

	// Line around the window to improve contrast against other black windows.
	GetForegroundDrawList()->AddRect(
		imguiRootWindow->ViewportPos,
		imguiRootWindow->ViewportPos + imguiRootWindow->Size,
		IM_COL32(255, 255, 255, 255));

	SetCursorPosY(70.0_scaled); /* <- height of the window header */
	DockSpace(GetID("AXEDockspace"));
	End();
}

}