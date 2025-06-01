#include "AXEAudioHelpers.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"

namespace Shadow::AXE {

void checkResult(ma_result r) {
	if (r == MA_SUCCESS) return;

	EC_ERROUT("All", "%s", ma_result_description(r));
	ERROUT("audio engine err: %s", ma_result_description(r));
}

/**
 * @brief Converts a duration in PCM frames, corresponding to a specific number of beats, to Beats Per Minute (BPM).
 *
 * @param pcmFrames The total number of PCM frames for the duration.
 * @param sampleRate The audio sample rate in Hz (samples per second).
 * @param numBeats The number of beats that the pcmFrames duration represents. For example, if pcmFrames is the length of one measure in 4/4 time, numBeats would be 4. If pcmFrames is the length of one beat, numBeats would be 1.
 * @return The calculated BPM as a double. Returns 0.0 if pcmFrames, sampleRate, or numBeats is zero, as BPM cannot be meaningfully calculated.
 */
double pcmFramesToBPM(uint64_t pcmFrames, uint32_t sampleRate, uint32_t numBeats) {
	if (pcmFrames == 0 || sampleRate == 0 || numBeats == 0) {
		return 0.0;
	}

	// Calculate the duration in seconds that the pcmFrames represent.
	// duration_seconds = total_frames / frames_per_second
	double durationInSeconds = static_cast<double>(pcmFrames) / static_cast<double>(sampleRate);

	if (durationInSeconds == 0.0) { // Should only happen if pcmFrames is 0, already checked, but good for safety.
		return 0.0;
	}

	// Calculate beats per second.
	// beats_per_second = total_beats / duration_seconds
	double beatsPerSecond = static_cast<double>(numBeats) / durationInSeconds;

	// Convert beats per second to beats per minute.
	// bpm = beats_per_second * 60
	return beatsPerSecond * 60.0;
}

/**
 * @brief Converts Beats Per Minute (BPM) to the number of PCM frames for a specific number of beats.
 *
 * @param bpm The tempo in Beats Per Minute.
 * @param sampleRate The audio sample rate in Hz (samples per second).
 * @param numBeats The number of beats for which to calculate the PCM frame count. For example, to find frames per measure in 4/4 time, numBeats would be 4. To find frames per beat, numBeats would be 1.
 * @return The calculated number of PCM frames as a uint64_t. Returns 0 if bpm is not positive or sampleRate is zero.
 */
uint64_t bpmToPCMFrames(double bpm, uint32_t sampleRate, uint32_t numBeats) {
	if (bpm <= 0.0 || sampleRate == 0) {
		return 0;
	}

	// Calculate the duration of a single beat in seconds.
	// seconds_per_beat = 60 / bpm
	double secondsPerBeat = 60.0 / bpm;

	// Calculate the total duration in seconds for the given number of beats.
	// total_duration_seconds = seconds_per_beat * num_beats
	double totalDurationInSeconds = secondsPerBeat * static_cast<double>(numBeats);

	// Calculate the total number of PCM frames for that duration.
	// total_frames = total_duration_seconds * frames_per_second (sampleRate)
	// Result is truncated towards zero, which is typical for frame counts.
	return static_cast<uint64_t>(totalDurationInSeconds * static_cast<double>(sampleRate));
}

}