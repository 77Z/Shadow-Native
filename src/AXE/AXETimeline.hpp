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

private:
	Song* songInfo;
	EditorState* editorState;
	ma_engine* audioEngine;

	uint64_t playbackFrames = 0;

};

}

#endif /* SHADOW_NATIVE_AXE_AXE_TIMELINE */