//
// Created by Vince on 4/5/22.
//

//TODO: Sound settings control from external file
//TODO: AXE Integration
//TODO:  3D/Atmos Support

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "shadow/audio.h"
#include <iostream>

ma_engine audioEngine;

namespace ShadowAudio {
    int initAudioEngine() {
        ma_result result;

        result = ma_engine_init(nullptr, &audioEngine);
        if (result != MA_SUCCESS) {
            std::cerr << "SHADOW FAILED TO INIT AUDIO ENGINE" << std::endl;
            return -1;
        }

        std::cout << "AUDIO ENGINE READY" << std::endl;

        //ma_engine_play_sound(&engine, "path", NULL);

        return 0;
    }

    void shutdownAudioEngine() {
        ma_engine_uninit(&audioEngine);
        std::cout << "AUDIO ENGINE SHUTDOWN" << std::endl;
    }
}