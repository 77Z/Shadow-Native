// This file is just a translation unit to be associated with AXETypes.hpp. The
// stuff you probably want is in that file.

#include "AXETypes.hpp"

namespace Shadow::AXE {

const char* audioFileTypes_FileExtensions[AudioFileTypes_Count] = {
	"",
	"wav",
	"flac",
	"mp3",
	"ogg",
};

const char* audioFileTypes_PrettyNames[AudioFileTypes_Count] = {
	"?? Unknown Audio Format ??",
	"Waveform Audio",
	"Free Lossless Audio Codec",
	"MPEG-1 Audio Layer III",
	"OGG / Vorbis Audio Codec",
};

const char* keysPretty[Keys_Count] = {
	"C",
	"C# / D♭",
	"D",
	"D# / E♭",
	"E",
	"F",
	"F# / G♭",
	"G",
	"G# / A♭",
	"A",
	"A# / B♭",
	"B"
};

}