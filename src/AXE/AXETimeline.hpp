#ifndef SHADOW_NATIVE_AXE_AXE_TIMELINE
#define SHADOW_NATIVE_AXE_AXE_TIMELINE

#include "AXETypes.hpp"

namespace Shadow::AXE {

class Timeline {
public:
	Timeline(Song* songInfo, EditorState* editorState);
	~Timeline();

	void onUpdate();

private:
	Song* songInfo;
	EditorState* editorState;

};

}

#endif /* SHADOW_NATIVE_AXE_AXE_TIMELINEk */