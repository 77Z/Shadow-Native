#ifndef SHADOW_NATIVE_AUDIO_HPP
#define SHADOW_NATIVE_AUDIO_HPP

#include "miniaudio.h"

namespace Shadow::Audio {

class AudioEngine {
public:
	AudioEngine();
	~AudioEngine();

	AudioEngine(const AudioEngine&) = delete;
	AudioEngine& operator=(const AudioEngine&) = delete;

	void playTestAudio();

private:
	ma_engine audioEngine;
};

}

#endif /* SHADOW_NATIVE_AUDIO_HPP */
