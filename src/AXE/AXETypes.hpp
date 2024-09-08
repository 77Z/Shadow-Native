#ifndef SHADOW_NATIVE_AXE_AXE_TYPES_HPP
#define SHADOW_NATIVE_AXE_AXE_TYPES_HPP

#include "../nodeEditor/imgui_node_editor.h"
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <string>
#include <utility>
#include <vector>
#include "imgui.h"
#include "miniaudio.h"
#include <memory>

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

enum NodeProperties_ {
	// Scalars
	NodeProperties_S8,       // signed char / char (with sensible compilers)
	NodeProperties_U8,       // unsigned char
	NodeProperties_S16,      // short
	NodeProperties_U16,      // unsigned short
	NodeProperties_S32,      // int
	NodeProperties_U32,      // unsigned int
	NodeProperties_S64,      // long long / __int64
	NodeProperties_U64,      // unsigned long long / unsigned __int64
	NodeProperties_Float,    // float
	NodeProperties_Double,   // double

	// Special prop types
	NodeProperties_Bool,     // bool
	NodeProperties_COUNT
};

struct NodeProperty {
	std::string propLabel = "!! UNTITLED PROP !!";
	std::string help = "";
	NodeProperties_ propType;
	std::shared_ptr<void> data;
	std::shared_ptr<void> min; // Used in num types
	std::shared_ptr<void> max; // Used in num types
	// formality for most data types given their static sizes
	size_t size;

};

enum NodeType_ {
	NodeType_Regular,
	NodeType_Output
};

struct Node {
	ma_node_base engineNodeBase; // Must be first member
	ed::NodeId id;
	std::string name;
	std::vector<Pin> inputs;
	std::vector<Pin> outputs;
	std::vector<NodeProperty> props;
	ImColor color;
	NodeType_ type;
	ImVec2 size;

	float x;
	float y;
};

struct NGComment {
	ed::NodeId id;
	std::string data = "New Comment";
	ImVec2 size = ImVec2(300, 200);
};

struct NodeGraph {
	std::string name;
	//TODO: update to a u64
	int lastKnownGraphId = 1000;
	// ed::EditorContext* ctx;
	// std::shared_ptr<ed::EditorContext> ctx;
	// std::shared_ptr<void> ctx;

	std::vector<Node> nodes;
	ImVector<Link> links;
	std::vector<NGComment> comments;

	std::time_t lastModified;
	std::time_t lastCompiled;

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
	uint64_t position;
	std::string name;
};

struct Automation {
	uint64_t startRail = 10;
	uint64_t endRail = 200;
	std::vector<std::pair<uint64_t, float>> points;
	ImColor color = ImColor(IM_COL32(255, 0, 0, 255));
	bool visible = true;
	bool smoothCurve = true;
};

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
	bool loaded = false;
};

struct Track {
	std::string name;
	std::vector<std::shared_ptr<Clip>> clips;
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

	bool snappingEnabled = true;
	
	float zoom = 100.0f;

	float lastKnownMasterVol = -1.0f;

	bool automationDebugMode = false;

	// Window states
	bool showShadowEngineConsole = true;
	bool showImGuiConsole = false;
	bool showImGuiMetrics = false;
	bool showImGuiStackTool = false;
	bool showClipboardBuffer = false;
	bool showProjectMetadata = false;
	bool showHelpDocs = false;
	bool showNodeEditor = true;
	bool showNodeEditorDebugger = true;
	bool showClipBrowser = true;
	bool showEqualizer = true;
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_TYPES_HPP */