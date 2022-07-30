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
#include "Logger.h"

namespace ShadowAudio {
    ma_engine audioEngine;
    int initAudioEngine() {
        ma_result result;

        result = ma_engine_init(nullptr, &audioEngine);
        if (result != MA_SUCCESS) {
            std::cerr << "SHADOW FAILED TO INIT AUDIO ENGINE" << std::endl;
            return -1;
        }

	print("AUDIO ENGINE READY");

        ma_engine_play_sound(&audioEngine, "./sound.wav", nullptr);

        return 0;
    }

    int simplePlayAudio(const char* filePath, bool looping) {
        ma_sound sound;
        ma_result result;

        //TODO: Compile audio files into .h maybe then load them through here, allowing audio files to compile into the binary.

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

        //TODO: see https://miniaud.io/docs/examples/engine_advanced.html
        ma_sound returnaudio;
        return returnaudio;
    }

    int playFetchedAudio(ma_sound sound) {
        //TODO: see https://miniaud.io/docs/examples/engine_advanced.html
        return 0;
    }

    void shutdownAudioEngine() {
        ma_engine_uninit(&audioEngine);
	print("AUDIO ENGINE SHUTDOWN");
    }
}
