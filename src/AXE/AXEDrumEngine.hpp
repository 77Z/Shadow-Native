#ifndef SHADOW_NATIVE_AXE_AXE_DRUM_ENGINE
#define SHADOW_NATIVE_AXE_AXE_DRUM_ENGINE

#include "AXETypes.hpp"
#include <vector>

namespace Shadow::AXE {

class AXEDrumEngine {
public:
	AXEDrumEngine(
		Song* songInfo,
		EditorState* editorState,
		ma_engine* audioEngine
	);
	~AXEDrumEngine();

	void onUpdate();

	void openDrumEditor(Clip* clip);

	void scheduleDrumPlayback(Clip* clip, uint64_t playbackTime);

private:
	Song* songInfo;
	EditorState* editorState;
	ma_engine* audioEngine;

	// Each opened drum collection in this vector will have its own window for
	// the user to edit the beat.
	std::vector<Clip*> openedDrumCollections;
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_DRUM_ENGINE */