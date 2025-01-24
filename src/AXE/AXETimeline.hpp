#ifndef SHADOW_NATIVE_AXE_AXE_TIMELINE
#define SHADOW_NATIVE_AXE_AXE_TIMELINE

#include "AXEClipBrowser.hpp"
#include "AXETypes.hpp"
#include "ShadowWindow.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <memory>

namespace Shadow::AXE {

class Timeline {
public:
	Timeline(Song* songInfo, EditorState* editorState, ma_engine* audioEngine, ShadowWindow* window);
	~Timeline();

	void onUpdate();

	void startPlayback();
	void stopPlayback();
	void togglePlayback();
	bool isPlaying() { return playing; }

private:
	Song* songInfo;
	EditorState* editorState;
	ma_engine* audioEngine;
	ShadowWindow* window;

	uint64_t playbackFrames = 0;
	bool playing = false;

	// std::shared_ptr<Clip> clipBeingDragged = nullptr;
	Clip* selectedClip = nullptr;
	Clip* clipBeingDragged = nullptr;
	float clipStoredMouseOffsetX = 0.0f;
	int tableHoveredRow = -1;
	Track* currentlySelectedTrack = nullptr;
	Automation* automationStartRailBeingDragged = nullptr;
	Automation* automationEndRailBeingDragged = nullptr;
	ImGuiID autoPointBeingDragged = 0;
	ImGuiSelectionBasicStorage clipSelection;

	ImRect selectionRect;

	void updateTrackAutomationsPopup();
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_TIMELINE */