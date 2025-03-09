#ifndef SHADOW_NATIVE_AXE_AXE_TIMELINE
#define SHADOW_NATIVE_AXE_AXE_TIMELINE

#include "AXEClipBrowser.hpp"
#include "AXETypes.hpp"
#include "ShadowWindow.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

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

	void newBookmark();
	void updateBookmarkDebugMenu(bool& p_open);

private:
	Song* songInfo;
	EditorState* editorState;
	ma_engine* audioEngine;
	ShadowWindow* window;

	uint64_t playbackFrames = 0;
	bool playing = false;

	// std::shared_ptr<Clip> clipBeingDragged = nullptr;
	// Clip* selectedClip = nullptr;
	// Clip* clipBeingDragged = nullptr;
	std::vector<Clip*> selectedClips;
	// std::vector<
	// 	std::pair<Clip*,   // Clip reference
	// 	float             //  clipStoredMouseOffsetX
	// >> clipsBeingDragged;

	// std::vector<Clip*> clipsBeingDragged;

	std::vector<
		std::pair<Clip*,   // Clip reference
		uint64_t          //  initial clip position at start of drag op
	>> clipsBeingDragged;

	bool actionClipsBeingDragged = false;
	bool globalStateIsNowDragging = false;
	float mouseXonDragStart = 0.0f;


	float clipStoredMouseOffsetX = 0.0f;

	int tableHoveredRow = -1;
	Track* currentlySelectedTrack = nullptr;
	Automation* automationStartRailBeingDragged = nullptr;
	Automation* automationEndRailBeingDragged = nullptr;
	ImGuiID autoPointBeingDragged = 0;
	ImGuiSelectionBasicStorage clipSelection;
	int bookmarkInc = 0;
	
	ImRect selectionRect;

	void updateTrackAutomationsPopup();

	void startClipDragging();
	// void stopClipDragging();

	void onUpdateBookmarks(ImDrawList* drawDest);
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_TIMELINE */