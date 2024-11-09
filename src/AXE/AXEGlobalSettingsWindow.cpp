#include "IconsCodicons.h"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "imgui_internal.h"

namespace Shadow::AXE {

typedef struct GlobalSettings {
	
} globalSettings;

static void HelpMarker(const char* desc) {
	using namespace ImGui;
	TextDisabled("(?)");
	if (BeginItemTooltip()) {
		PushTextWrapPos(GetFontSize() * 35.0f);
		TextUnformatted(desc);
		PopTextWrapPos();
		EndTooltip();
	}
}

void onUpdateGlobalSettingsWindow(bool* p_open) {
	using namespace ImGui;

	if (!p_open) return;

	if (!Begin("AXE Global Settings", p_open, ImGuiWindowFlags_NoCollapse)) {
		End();
		return;
	}

	float winHeight = GetWindowHeight();
	float winWidth = GetWindowWidth();
	ImDrawList* wdl = GetWindowDrawList();

	TextCenter("Waveform Render Style");
	SameLine(); HelpMarker(
		"This detail is only visible when you zoom in significantly"
		" anywhere where waveforms are displayed");

	ImVec2 cursorScreen = GetCursorScreenPos();
	
	ImRect firstBoxRect = ImRect(cursorScreen, ImVec2(cursorScreen.x + (winWidth / 2) - 10, cursorScreen.y + 200));
	wdl->AddRect(firstBoxRect.Min, firstBoxRect.Max, IM_COL32(255, 255, 255, 50), 6, 0, 3);
	if (IsMouseHoveringRect(firstBoxRect.Min, firstBoxRect.Max) && IsWindowHovered()) {
		wdl->AddRectFilled(firstBoxRect.Min, firstBoxRect.Max, IM_COL32(255, 255, 255, 70), 6);
	}

	PushClipRect(firstBoxRect.Min, firstBoxRect.Max, true);

	// Draw waveforms

	PopClipRect();

	ImRect secondBoxRect = ImRect(ImVec2(cursorScreen.x + (winWidth / 2) + 10, cursorScreen.y), ImVec2(cursorScreen.x + winWidth - 20, cursorScreen.y + 200));
	wdl->AddRect(secondBoxRect.Min, secondBoxRect.Max, IM_COL32(255, 255, 255, 50), 6, 0, 3);
	if (IsMouseHoveringRect(secondBoxRect.Min, secondBoxRect.Max) && IsWindowHovered()) {
		wdl->AddRectFilled(secondBoxRect.Min, secondBoxRect.Max, IM_COL32(255, 255, 255, 70), 6);
	}
	PushClipRect(secondBoxRect.Min, secondBoxRect.Max, true);

	// Draw waveforms

	PopClipRect();


	SetCursorPosY(winHeight - GetFrameHeightWithSpacing() - GetStyle().ItemSpacing.y);
	SetCursorPosX(winWidth - CalcTextSize(ICON_CI_SAVE_ALL " SAVE SETTINGS").x - GetStyle().ItemSpacing.x * 2);
	Button(ICON_CI_SAVE_ALL " SAVE SETTINGS");

	End();
}

}