//
// Created by Vince on 4/5/22.
//

#ifndef SHADOW_NATIVE_AUDIO_H
#define SHADOW_NATIVE_AUDIO_H

#include "miniaudio.h"

namespace ShadowAudio {
	// @brief Starts up Shadow Audio Engine
	// @return if 0, success, not 0, fail
	int initAudioEngine();

	ma_result playTestAudio();

	// @brief Shutdown Shadow Audio Engine
	void shutdownAudioEngine();
}

#endif //SHADOW_NATIVE_AUDIO_H
