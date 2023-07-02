#ifndef SHADOW_NATIVE_AUDIO_H
#define SHADOW_NATIVE_AUDIO_H

#include "miniaudio.h"

namespace Shadow {
namespace Audio {

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
}

#endif // SHADOW_NATIVE_AUDIO_H
