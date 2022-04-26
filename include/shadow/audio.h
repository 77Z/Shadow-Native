//
// Created by Vince on 4/5/22.
//

#ifndef SHADOW_NATIVE_AUDIO_H
#define SHADOW_NATIVE_AUDIO_H

namespace ShadowAudio {
    // @brief Starts up Shadow Audio Engine
    // @return if 0, success, not 0, fail
    int initAudioEngine();

    // @brief Shutdown Shadow Audio Engine
    void shutdownAudioEngine();
}

#endif //SHADOW_NATIVE_AUDIO_H
