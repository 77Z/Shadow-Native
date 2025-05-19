#include "AXEDrumEngine.hpp"
#include "AXETypes.hpp"
#include "ShadowIcons.hpp"
#include "imgui.h"
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

		TextUnformatted("DRUM MACHINE!!");

		if (Button("Done")) removeMeFromOpened();

		End();

		it++;
	}
}

}