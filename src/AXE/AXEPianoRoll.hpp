#ifndef SHADOW_NATIVE_AXE_AXE_PIANO_ROLL_HPP
#define SHADOW_NATIVE_AXE_AXE_PIANO_ROLL_HPP

#include "AXETypes.hpp"
#include <vector>

namespace Shadow::AXE {

class PianoRoll {
public:
	PianoRoll(
		Song* songInfo,
		EditorState* editorState,
		ma_engine* audioEngine
	);
	~PianoRoll();

	void openPianoRoll(Clip* clip);

	void onUpdate();

private:
	Song* songInfo;
	EditorState* editorState;
	ma_engine* audioEngine;

	std::vector<Clip*> openedPianoRolls;
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_PIANO_ROLL_HPP */