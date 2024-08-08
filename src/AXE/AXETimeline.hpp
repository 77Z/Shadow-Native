#ifndef SHADOW_NATIVE_AXE_AXE_TIMELINE
#define SHADOW_NATIVE_AXE_AXE_TIMELINE

#include "AXEClipBrowser.hpp"
#include "AXETypes.hpp"

namespace Shadow::AXE {

class Timeline {
public:
	Timeline(Song* songInfo, EditorState* editorState, ma_engine* audioEngine);
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

	uint64_t playbackFrames = 0;
	bool playing = false;

	Clip* clipBeingDragged = nullptr;
	float clipStoredMouseOffsetX = 0.0f;

};

}

#endif /* SHADOW_NATIVE_AXE_AXE_TIMELINE */