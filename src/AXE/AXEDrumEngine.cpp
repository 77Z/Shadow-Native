#include "AXEDrumEngine.hpp"
#include "AXETypes.hpp"
#include "Debug/EditorConsole.hpp"
#include "ShadowIcons.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <memory>
#include <string>

namespace Shadow::AXE {

AXEDrumEngine::AXEDrumEngine() { }

AXEDrumEngine::~AXEDrumEngine() { }

void AXEDrumEngine::openDrumEditor(Clip* clip) {
	if (clip == nullptr) return;

	openedDrumCollections.emplace_back(clip);
}

void AXEDrumEngine::onUpdate() {
	using namespace ImGui;

	// Don't waste time doing anything else if there are no drums to edit
	if (openedDrumCollections.empty()) return;

	// currentlySelectedTrack->automations.erase(currentlySelectedTrack->automations.begin() + autoIt - 1)

	int it = 0;
	for (auto clip : openedDrumCollections) {
		
		auto removeMeFromOpened = [this, it]() {
			openedDrumCollections.erase(openedDrumCollections.begin() + it);
		};

		if (clip == nullptr) removeMeFromOpened();
		if (clip->clipType != TimelineClipType_Drums) removeMeFromOpened();

		// initialize drum data if it doesn't exist already
		if (clip->drumData == nullptr) clip->drumData = std::make_shared<DrumMachineData>();

		std::string windowTitle =
			SHADOW_ICON_DRUMS " Drum Machine " SHADOW_ICON_ARROW_RIGHT + clip->name + "##" + std::to_string(it);

		if (!Begin(windowTitle.c_str())) {
			End();
			it++;
			continue;
		}

		PushID(it);

		auto winPos = GetWindowPos();
		auto winSize = GetWindowSize();
		auto fg = GetForegroundDrawList();
		auto mouse = GetMousePos();

		if (BeginDragDropTargetCustom(ImRect(winPos, winPos + winSize), GetID("DrumMachineDropTarget"))) {
			fg->AddCircleFilled(GetMousePos(), 5.0f, IM_COL32(0, 255, 0, 255));
			std::string text = "Add clip to drum machine " + clip->name;
			fg->AddText(ImVec2(mouse.x, mouse.y - 40.0f), IM_COL32(0, 255, 0, 255), text.c_str());

			if (const ImGuiPayload* payload = AcceptDragDropPayload("AXE_CLIP_PATH_PAYLOAD")) {
				// ma_result res;
				// ma_uint32 soundFlags = MA_SOUND_FLAG_NO_SPATIALIZATION;
				// if (songInfo->decodeOnLoad) soundFlags |= MA_SOUND_FLAG_DECODE;

				const char* clipPath = static_cast<const char*>(payload->Data);
				// std::string clipPathString(clipPath, clipPath + payload->DataSize);
				EC_PRINT("All", "dropped %s", clipPath);
			}

			EndDragDropTarget();
		}

		TextUnformatted("DRUM MACHINE!!");

		if (Button("Done")) removeMeFromOpened();

		PopID();

		End();

		it++;
	}
}

}