#ifndef SHADOW_NATIVE_AXE_AXE_TYPES_HPP
#define SHADOW_NATIVE_AXE_AXE_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "miniaudio.h"

namespace Shadow::AXE {


enum Keys_ {
	Keys_C,
	Keys_CSharpDFlat,
	Keys_D,
	Keys_DSharpEFlat,
	Keys_E,
	Keys_F,
	Keys_FSharpGFlat,
	Keys_G,
	Keys_GSharpAFlat,
	Keys_A,
	Keys_ASharpBFlat,
	Keys_B,

	Keys_Count
};

struct NodeGraph {
	ma_node_graph compiledGraph;
};

struct Bookmark {
	float position;
	std::string name;
};

struct Automation {};

struct Clip {
	std::string name = "";
	std::string baseAudioSource = "";
	// std::vector<Automation> automations;

	float position = 0;

	float balence = 0.0f;	// -1 L : +1 R
	float volume = 100.0f;	// 0 - 100

	bool muted = false;
};

struct Track {
	std::string name;
	std::vector<Clip> clips;
	std::vector<Automation> automations;
	std::vector<Bookmark> bookmarks;

	float balence = 0.0f;	// -1 L : +1 R
	float volume = 100.0f;	// 0 - 100

	bool muted = false;
	// should solo be here??
};

struct Song {
	std::string name = "";
	std::string artist = "";
	std::string album = "";

	bool decodeOnLoad = true;
	// Async loading and/or streaming?

	float bpm = 120.0f;
	int key = 0;
	int timeSignature[2] = { 4, 4 };
	float masterVolume = 1.0f; // 0-1 with anything over 1 amplifying

	std::vector<Track> tracks;
	std::vector<NodeGraph> nodeGraphs;
};

struct EditorState {
	float sf;

	float lastKnownMasterVol = -1.0f;

	// Window states
	bool showShadowEngineConsole = false;
	bool showImGuiConsole = false;
	bool showImGuiMetrics = false;
	bool showImGuiStackTool = false;
	bool showClipboardBuffer = false;
	bool showProjectMetadata = false;
	bool showHelpDocs = false;
	bool showNodeEditor = true;
	bool showNodeEditorDebugger = false;
	bool showClipBrowser = true;
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_TYPES_HPP */