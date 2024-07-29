#include "AXETypes.hpp"
#include "Debug/EditorConsole.hpp"
#include "miniaudio.h"

#define EC_THIS "Song Bootstrapper"

namespace Shadow::AXE {

void bootstrapSong(const Song* song, const ma_engine* audioEngine) {
	EC_NEWCAT(EC_THIS);
	EC_PRINT(EC_THIS, "Bootstrapping song: %s with ShadowAudio", song->name.c_str());

	ma_uint32 soundFlags = MA_SOUND_FLAG_NO_SPATIALIZATION;
	if (song->decodeOnLoad) {
		soundFlags |= MA_SOUND_FLAG_DECODE;
	}

}

}