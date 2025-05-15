#include "AXEGlobalSettingsWindow.hpp"
#include "Configuration/EngineConfiguration.hpp"
#include "Debug/EditorConsole.hpp"
#include "ShadowIcons.hpp"
#include "ImGuiNotify.hpp"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "imgui_internal.h"
#include "json_impl.hpp"
#include <filesystem>
#include <string>

namespace Shadow::AXE {

static GlobalSettings globalSettings;

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

static void saveSettings() {
	json j;
	j["spp"] = globalSettings.samplesPerPixel;
	j["wr"] = globalSettings.wavyRender;

	JSON::dumpJsonToBson(j, EngineConfiguration::getConfigDir() + "/AXESettings.sec");
}

void loadGlobalSettingsFromDisk() {
	std::string settingsFile = EngineConfiguration::getConfigDir() + "/AXESettings.sec";

	if (!std::filesystem::exists(settingsFile)) {
		EC_PRINT("All", "No AXE global config, defaults will be used");
		return;
	}

	json j = JSON::readBsonFile(settingsFile);

	globalSettings.samplesPerPixel = j["spp"];
	globalSettings.wavyRender = j["wr"];
}

GlobalSettings* getGlobalSettings() { return &globalSettings; }

void onUpdateGlobalSettingsWindow(bool& p_open) {
	using namespace ImGui;

	if (!p_open) return;

	if (!Begin("AXE Global Settings", &p_open, ImGuiWindowFlags_NoCollapse)) {
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

	SetCursorPosY(GetCursorPosY() + 210);

	BeginDisabled();
	TextUnformatted("Waveform Resolution");
	SameLine();
	HelpMarker(
		"Excluding fonts and icons, everything in AXE is rendered as triangles. Your graphics card "
		"has to render poterntially tens or hundreds of thousands of triangles every single frame, "
		"and waveforms contribute significantly to this number. For more acurate waveforms, you "
		"want more samples, but more samples = more triangles for your poor GPU (There are already "
		"lots of culling optimizations). If you want higher resolution waveforms, crank up Samples "
		"Per Pixel. If your computer starts to hate you after you have a decent amount of clips, "
		"save your project and lower it a bit.");
	//TODO: What is a reasonable max for this?
	SliderInt("Samples Per Pixel", &globalSettings.samplesPerPixel, 1, 1024);
	EndDisabled();
	HelpMarker("This is broken at the moment. whoops.");


	SetCursorPosY(winHeight - GetFrameHeightWithSpacing() - GetStyle().ItemSpacing.y);
	SetCursorPosX(winWidth - CalcTextSize(SHADOW_ICON_SAVE_ALL " SAVE SETTINGS").x - GetStyle().ItemSpacing.x * 2);
	if (Button(SHADOW_ICON_SAVE_ALL " SAVE SETTINGS")) {
		saveSettings();
		InsertNotification({ImGuiToastType::Success, 5000, "Global settings saved"});
	}

	End();
}

}