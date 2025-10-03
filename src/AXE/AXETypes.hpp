#ifndef SHADOW_NATIVE_AXE_AXE_TYPES_HPP
#define SHADOW_NATIVE_AXE_AXE_TYPES_HPP

#include "../nodeEditor/imgui_node_editor.h"
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>
#include "imgui.h"
#include "miniaudio.h"
#include <memory>
#include "JUCE.hpp"

namespace Shadow::AXE {

namespace ed = ax::NodeEditor;
namespace fs = std::filesystem;

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

	ed::Config config;
	ed::EditorContext* editorContext;
	
	std::vector<Node> nodes;
	ImVector<Link> links;
	std::vector<NGComment> comments;

	std::time_t lastModified;
	std::time_t lastCompiled;

	ma_node_graph compiledGraph;
};

enum AudioFileTypes_ {
	AudioFileTypes_Unknown,
	AudioFileTypes_wav,
	AudioFileTypes_flac,
	AudioFileTypes_mp3,
	AudioFileTypes_ogg,

	AudioFileTypes_Count
};

extern const char* audioFileTypes_FileExtensions[AudioFileTypes_Count];
extern const char* audioFileTypes_PrettyNames[AudioFileTypes_Count];

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

extern const char* keysPretty[Keys_Count];

enum TimelineUnit_ {
	TimelineUnit_BPM,
	TimelineUnit_TimeScale,
	TimelineUnit_PCMFrames,

	TimelineUnit_Count
};

struct Bookmark {
	uint64_t position;
	std::string name;
	ImColor color;
};

struct Automation {
	uint64_t startRail = 10;
	uint64_t endRail = 200;
	// u64 is the pcm frame index, float is a percent applied 0...1.0f
	std::vector<std::pair<uint64_t, float>> points;
	ImColor color = ImColor(IM_COL32(255, 0, 0, 255));
	bool visible = true;
	bool smoothCurve = true;
};

struct DrumTrack {
	fs::path samplePath;
	std::vector<bool> beats;
};

struct DrumMachineData {
	std::vector<DrumTrack> drumTracks;
	uint32_t measures = 1;

	// Volatile data
	ma_sound_group drumSoundGroup;
};

struct PianoRollData {
	// The piano roll uses baseAudioSource for its sample. So no sources is
	// stored in this struct unlike the DrumMachineData.

	// Volatile data
	ma_sound_group pianoSoundGroup;
};

enum TimelineClipType_ {
	TimelineClipType_Audio,
	TimelineClipType_Drums,
	TimelineClipType_PianoRoll,

	TimelineClipType_COUNT
};

struct Clip {
	std::string name = "";
	std::string baseAudioSource = "";

	uint64_t position = 0;
	// uint64_t length = 0;
	unsigned long long length = 0;

	float balance = 0.0f;	// -1 L : +1 R
	float volume = 100.0f;	// 0 - 100

	bool muted = false;

	TimelineClipType_ clipType = TimelineClipType_Audio;
	// WILL BE nullptr if NOT TYPE DRUMS
	std::shared_ptr<DrumMachineData> drumData = nullptr;
	// WILL BE nullptr if NOT TYPE PIANO ROLL
	std::shared_ptr<PianoRollData> pianoRollData = nullptr;

	// Volatile data
	ma_sound engineSound;
	bool loaded = false;
	bool shouldDrawWaveform = true;
	std::vector<int16_t> waveformData;
	int waveformChannels = 0;
	juce::AudioFormatManager audioFormatManager;
	juce::AudioThumbnailCache thumbnailCache{ 5 };
	juce::AudioThumbnail thumbnail{100, audioFormatManager, thumbnailCache};
};

struct Track {
	std::string name;
	std::vector<std::shared_ptr<Clip>> clips;
	std::vector<Automation> automations;

	float balance = 0.0f;	// -1 L : +1 R
	float volume = 100.0f;	// 0 - 100

	bool muted = false;

	// When false, the balance slider is just regular balance. When true, it's a
	// true panning effect: The sound from one side will "move" to the other
	// side and blend with it.
	bool realPanning = false;

	ImColor color;

	// Volatile data
	ma_sound_group soundGroup;
};

struct Song {
	int songFileVersion = 1;

	std::string name = "";
	std::string artist = "";
	std::string album = "";

	bool decodeOnLoad = true;
	// Async loading and/or streaming?

	float bpm = 120.0f;
	int key = 0;
	int timeSignature[2] = { 4, 4 };
	float masterVolume = 1.0f; // 0-1 with anything over 1 amplifying
	TimelineUnit_ timelineUnits = TimelineUnit_BPM;

	uint64_t lastKnownGraphId = 10000;

	uint64_t songLength = 10000; // in PCM frames

	std::vector<Track> tracks;
	std::vector<NodeGraph> nodeGraphs;
	std::vector<Bookmark> bookmarks;
};

struct EditorState {
	float sf;

	uint32_t sampleRate = 0;

	bool snappingEnabled = true;
	
	// 100.0f is default zoom level. Usually implementors using zoom prefer to
	// divide zoom by 100.0f so that default zoom is 1.0f
	float zoom = 100.0f;

	bool automationDebugMode = false;

	bool timelinePositionDebugMode = false;

	ImFont* headerFont;
	ImFont* segmentDisplayFont;

	Track* soloedTrack;

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
	bool showGlobalSettings = false;
	bool showPianoRoll = true;
	bool showBookmarksDebugger = true;
	bool showIconDebugger = false;
	bool showVSTWindow = false;
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_TYPES_HPP */