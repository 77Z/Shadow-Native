#ifndef SHADOW_NATIVE_AXE_AXE_TYPES_HPP
#define SHADOW_NATIVE_AXE_AXE_TYPES_HPP

#include <string>
#include <vector>

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

struct Bookmark {
	float position;
	std::string name;
};

struct Automation {};

struct Clip {
	std::string name;
	std::string baseAudioSource;
	// std::vector<Automation> automations;
};

struct Track {
	std::string name;
	std::vector<Clip> clips;
	std::vector<Automation> automations;
	std::vector<Bookmark> bookmarks;

	float balence = 0.0f;	// -1 L : +1 R
	float volume = 100.0f;	// 0 - 100
};

struct Song {
	float bpm = 120.0f;
	int key;
	int timeSignature[2] = { 4, 4 };

	std::vector<Track> tracks;
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_TYPES_HPP */