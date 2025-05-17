#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "AXETypes.hpp"
#include <cstdint>
#include <string>

namespace Shadow::AXE {

enum ScrubberClockWidgetStyle_ {
	ScrubberClockWidgetStyle_PCMFrames,
	ScrubberClockWidgetStyle_BPM
};

static ScrubberClockWidgetStyle_ style = ScrubberClockWidgetStyle_PCMFrames;

void drawScrubberClockWidget(Song& song, EditorState& edState, uint64_t playbackFrames) {
	using namespace ImGui;

	PushFont(edState.segmentDisplayFont);

	std::string displayText;

	if (style == ScrubberClockWidgetStyle_PCMFrames) {
		displayText = std::to_string(playbackFrames * 100);
	} else if (style == ScrubberClockWidgetStyle_BPM) {

		// 1 PCM frame is 1 sample per channel.
		// AXE Audio is 2 samples per channel. So 1 PCM frame has 2 samples.

		// There's typically 48000 PCM frames per second. (96000 samples / second)
		auto framePerSecond = edState.sampleRate / 2;

		// BPM = Beats per minute
		float bpm = song.bpm;

		// BPS = Beats per second
		float bps = bpm / 60;

		auto samplesPerBeat = framePerSecond / bps;

		TextUnformatted(std::string(std::to_string(samplesPerBeat)).c_str());
		SameLine();

		auto beatsPassed = (playbackFrames * 100) / samplesPerBeat;

		displayText = std::to_string(beatsPassed);

	}

	auto textSize = CalcTextSize(displayText.c_str());

	// auto dl = GetWindowDrawList();
	// auto cursor = GetCursorScreenPos();

	// dl->AddRectFilled(cursor, cursor + textSize, IM_COL32(34, 34, 34, 255));

	if (InvisibleButton("##ClockWidget", textSize))
		OpenPopup("ScrubberClockWidgetStylePopup");

	SameLine();

	TextUnformatted(displayText.c_str());

	if (style == ScrubberClockWidgetStyle_BPM) {
		SameLine();
		Text(" at %f BPM", song.bpm);
	} else if (style == ScrubberClockWidgetStyle_PCMFrames) {
		SameLine();
		TextUnformatted(" PCM");
	}

	PopFont();

	if (BeginPopup("ScrubberClockWidgetStylePopup")) {

		if (RadioButton("PCM Frames",
			style == ScrubberClockWidgetStyle_PCMFrames))
			style = ScrubberClockWidgetStyle_PCMFrames;

		if (RadioButton("BPM",
			style == ScrubberClockWidgetStyle_BPM))
			style = ScrubberClockWidgetStyle_BPM;

		EndPopup();
	}

}

}