//
// Created by vince on 2/12/26.
//

#ifndef SHADOW_NATIVE_AXE_EDITOR_HEADER_HPP
#define SHADOW_NATIVE_AXE_EDITOR_HEADER_HPP

#include <string>

class ShadowWindow;
struct ma_engine;

namespace Shadow::AXE {

class Timeline;
class Song;
class EditorState;

void headerBarLoad();

void drawHeaderBar(Song* songInfo, EditorState* editorState, ShadowWindow* window, Timeline* timeline, ma_engine engine, const std::string& projectFile);

}

#endif // SHADOW_NATIVE_AXE_EDITOR_HEADER_HPP
