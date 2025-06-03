#include "AXETypes.hpp"
#include "Debug/EditorConsole.hpp"
#include "miniaudio.h"
#include <cstddef>
#include "AXEAudioHelpers.hpp"

#define EC_THIS "Song Bootstrapper"

namespace Shadow::AXE {

// TODO: make this better.
void bootstrapSong(Song* song, ma_engine* audioEngine) {
	EC_NEWCAT(EC_THIS);
	EC_PRINT(EC_THIS, "Bootstrapping song: %s with ShadowAudio", song->name.c_str());

	ma_result result;

	ma_uint32 soundFlags = MA_SOUND_FLAG_NO_SPATIALIZATION;
	if (song->decodeOnLoad) {
		soundFlags |= MA_SOUND_FLAG_DECODE;
	}

	for (auto& track : song->tracks) {
		result = ma_sound_group_init(audioEngine, soundFlags, nullptr, &track.soundGroup);
		if (result != MA_SUCCESS) {
			EC_ERROUT(EC_THIS, "FAILED TO INIT SOUND GROUP FOR TRACK %s", track.name.c_str());
			continue;
		}

		checkResult(result);

		ma_sound_group_set_volume(&track.soundGroup, (track.volume / 100.0f));
		ma_sound_group_set_pan_mode(&track.soundGroup, track.realPanning ? ma_pan_mode_pan : ma_pan_mode_balance);
		ma_sound_group_set_pan(&track.soundGroup, track.balance);

		if (track.muted) ma_sound_group_set_volume(&track.soundGroup, 0.0f);

	}


	for (auto& track : song->tracks) {
		for (auto& clip : track.clips) {

			if (clip->clipType == TimelineClipType_Drums) {
				result = ma_sound_group_init(audioEngine, soundFlags, &track.soundGroup, &clip->drumData->drumSoundGroup);
				checkResult(result);
			}

			if (clip->clipType == TimelineClipType_PianoRoll) {
				result = ma_sound_group_init(audioEngine, soundFlags, &track.soundGroup, &clip->pianoRollData->pianoSoundGroup);
				checkResult(result);
			}

			if (clip->clipType == TimelineClipType_PianoRoll || clip->clipType == TimelineClipType_Drums) {
				// No sound to load, just continue
				clip->loaded = true;
				continue;
			}

			result = ma_sound_init_from_file(audioEngine, clip->baseAudioSource.c_str(), soundFlags, &track.soundGroup, nullptr, &clip->engineSound);
			clip->loaded = true;
			if (result != MA_SUCCESS) {
				EC_ERROUT(EC_THIS, "FAILED TO INIT SOUND FOR CLIP %s", clip->name.c_str());
				EC_ERROUT(EC_THIS, "Clip base audio source %s", clip->baseAudioSource.c_str());
			}
		}
	}
}

void unloadSong(Song* song, ma_engine* audioEngine) {
	for (auto& track : song->tracks) {
		for (auto& clip : track.clips) {
			ma_sound_uninit(&clip->engineSound);
			clip->loaded = false;
		}
	}
}
}