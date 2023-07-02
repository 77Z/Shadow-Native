// TODO: Sound settings control from external file
// TODO: AXE Integration
// TODO:  3D/Atmos Support

#define MINIAUDIO_IMPLEMENTATION
#include "shadow/audio.h"
#include "Logger.h"
#include "miniaudio.h"

namespace Shadow {
namespace Audio {

	AudioEngine::AudioEngine() {
		ma_result result = ma_engine_init(nullptr, &audioEngine);

		if (result != MA_SUCCESS) {
			ERROUT("Failed to start AudioEngine");
			return;
		}

		PRINT("AudioEngine ready");
	}

	AudioEngine::~AudioEngine() {
		ma_engine_uninit(&audioEngine);
		PRINT("AudioEngine shutdown");
	}

	void AudioEngine::playTestAudio() {
		ma_engine_play_sound(&audioEngine, "./sound.wav", nullptr);
	}

}
/*
int simplePlayAudio(const char* filePath, bool looping) {
	ma_sound sound;
	ma_result result;

	// TODO: Compile audio files into .h maybe then load them through here, allowing audio files to
	// compile into the binary.

	result = ma_sound_init_from_file(&audioEngine, filePath, 0, nullptr, nullptr, &sound);
	if (result != MA_SUCCESS) {
		std::cerr << "SHADOW FAILED TO LOAD SOUND FILE" << std::endl;
		return -1;
	}

	ma_sound_start(&sound);

	return 0;
}

ma_sound prefetchAudio(const char* filepath) {
	// Allows fetching the audio beforehand so that it's ready
	// to go in memory and plays fast, this is the good way to do it

	// TODO: see https://miniaud.io/docs/examples/engine_advanced.html
	ma_sound returnaudio;
	return returnaudio;
}

int playFetchedAudio(ma_sound sound) {
	// TODO: see https://miniaud.io/docs/examples/engine_advanced.html
	return 0;
}*/

}
