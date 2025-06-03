#ifndef SHADOW_NATIVE_AXE_AXE_TIMELINE
#define SHADOW_NATIVE_AXE_AXE_TIMELINE

#include "AXEClipBrowser.hpp"
#include "AXEDrumEngine.hpp"
#include "AXENodeEditor.hpp"
#include "AXEPianoRoll.hpp"
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
	Timeline(
		Song* songInfo,
		EditorState* editorState,
		ma_engine* audioEngine,
		ShadowWindow* window,
		AXENodeEditor* nodeEditor,
		AXEDrumEngine* drumEngine,
		PianoRoll* pianoRoll
	);
	~Timeline();

	void onUpdate();

	void startPlayback();
	void stopPlayback();
	void togglePlayback();
	bool isPlaying() { return playing; }
	/// Returns PCM frame count for the main playback engine.
	uint64_t getPlaybackFrames() { return playbackFrames; }

	void newBookmark();
	void updateBookmarkDebugMenu(bool& p_open);

	void addTrack(const std::string& name = "Untitled Track");

	bool singleSlicingClip = false;
	bool multiSlicingClip = false;

private:
	Song* songInfo;
	EditorState* editorState;
	ma_engine* audioEngine;
	ShadowWindow* window;
	AXENodeEditor* nodeEditor;
	AXEDrumEngine* drumEngine;
	PianoRoll* pianoRoll;

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

	void newDrumCollection(Track* track, uint64_t position);
	void newPianoRoll(Track* track, uint64_t position);
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_TIMELINE */