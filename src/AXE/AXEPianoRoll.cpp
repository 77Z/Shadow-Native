#include "AXEPianoRoll.hpp"
#include "Debug/EditorConsole.hpp"
#include "imgui.h"
#include "ShadowIcons.hpp"
#include <string>

#define EC_THIS "Piano Roll"

namespace Shadow::AXE {

PianoRoll::PianoRoll(
	Song* songInfo,
	EditorState* editorState,
	ma_engine* audioEngine
)
	: songInfo(songInfo)
	, editorState(editorState)
	, audioEngine(audioEngine)
{
	EC_NEWCAT(EC_THIS);
}

PianoRoll::~PianoRoll() { }

void PianoRoll::openPianoRoll(Clip* clip) {
	if (clip == nullptr) return;

	openedPianoRolls.emplace_back(clip);
}

void PianoRoll::onUpdate() {
	using namespace ImGui;

	// Don't waste time doing anything else if there are no piano rolls to edit
	if (openedPianoRolls.empty()) return;


	int it = 0;
	for (auto clip : openedPianoRolls) {

		auto removeMeFromOpened = [this, it]() {
			openedPianoRolls.erase(openedPianoRolls.begin() + it);
		};

		if (clip == nullptr) removeMeFromOpened();
		if (clip->clipType != TimelineClipType_PianoRoll) removeMeFromOpened();

		// initialize piano roll data if it doesn't exist already
		if (clip->pianoRollData == nullptr) clip->pianoRollData = std::make_shared<PianoRollData>();


		std::string windowTitle = SHADOW_ICON_PIANO " Piano Roll " SHADOW_ICON_ARROW_RIGHT + clip->name + "##" + std::to_string(it);

		PushStyleColor(ImGuiCol_Border, IM_COL32(255, 0, 0, 255));
		PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
		if (!Begin(windowTitle.c_str(), nullptr, ImGuiWindowFlags_MenuBar)) {
			PopStyleVar();
			PopStyleColor();
			End();
			it++;
			continue;
		}
		PopStyleVar();
		PopStyleColor();

		PushID(it);

		if (BeginMenuBar()) {
			if (MenuItem("Close")) removeMeFromOpened();
			EndMenuBar();
		}

		if (clip->baseAudioSource.empty()) {
			// Center text in the window
			auto winSize = GetWindowSize();
			const char txt[] = "Drop an audio file here to use as sample for the piano roll";
			auto txtSize = CalcTextSize(txt);
			SetCursorPos(ImVec2((winSize.x - txtSize.x) / 2.0f, (winSize.y - txtSize.y) / 2.0f));
			TextUnformatted(txt);
		} else {
			// Sample loaded, show piano roll UI

			auto drawOctave = []() {

			};

			ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders;

			if (BeginTable("PianoRollTable", 2, tableFlags)) {

				EndTable();
			}
		}

		PopID();
	
		End();

		it++;
	}

}

}