#ifndef SHADOW_NATIVE_AXE_AXE_TYPES_HPP
#define SHADOW_NATIVE_AXE_AXE_TYPES_HPP

#include "../nodeEditor/imgui_node_editor.h"
#include <cstdint>
#include <string>
#include <vector>
#include "imgui.h"
#include "miniaudio.h"

namespace Shadow::AXE {

namespace ed = ax::NodeEditor;

struct Node;

enum PinType_ {};

enum PinKind_ {
	PinKind_Input,
	PinKind_Output,
	PinKind_COUNT,
};

struct Pin {
	ed::PinId id;
	Node* node;
	std::string name;
	PinType_ type;
	PinKind_ kind;
};

struct Link {
	ed::LinkId id;
	ed::PinId inputId;
	ed::PinId outputId;
};

enum NodeType_ {
	NodeType_Regular,
};

struct Node {
	ed::NodeId id;
	std::string name;
	std::vector<Pin> inputs;
	std::vector<Pin> outputs;
	ImColor color;
	NodeType_ type;
	ImVec2 size;
};

struct NodeGraph {
	std::string name;

	std::vector<Node> nodes;
	ImVector<Link> links;

	ma_node_graph compiledGraph;
};

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

struct Bookmark {
	float position;
	std::string name;
};

struct Automation {};

struct Clip {
	std::string name = "";
	std::string baseAudioSource = "";
	// std::vector<Automation> automations;

	uint64_t position = 0;
	uint64_t length = 0;

	float balence = 0.0f;	// -1 L : +1 R
	float volume = 100.0f;	// 0 - 100

	bool muted = false;

	// Volatile data
	ma_sound engineSound;
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

	// Volatile data
	ma_sound_group* soundGroup;
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