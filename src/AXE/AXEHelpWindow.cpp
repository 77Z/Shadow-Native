#include "imgui.h"

namespace Shadow::AXE {
void updateHelpWindow(bool& p_open) {
	using namespace ImGui;

	if (!p_open)
		return;

	Begin("ShadowAudio Help", &p_open);

	TextWrapped("The build of ShadowAudio included in this software currently supports the "
				"following backends:");

	TextUnformatted("Windows");
	BulletText("WASAPI");
	BulletText("DirectSound");
	BulletText("WinMM");

	TextUnformatted("Linux");
	BulletText("ALSA");
	BulletText("PulseAudio");
	BulletText("JACK");

	TextUnformatted("MacOS / iOS");
	BulletText("Core Audio");

	TextUnformatted("BSDs");
	BulletText("sndio");
	BulletText("audio(4)");
	BulletText("OSS");

	TextUnformatted("Android");
	BulletText("AAudio");
	BulletText("OpenSL");

	Separator();
	TextWrapped("This software also currently supports decoding the following audio formats:");
	BulletText("wav (Waveform Audio File)");
	BulletText("FLAC");
	BulletText("MP3");

	SeparatorText("Techinal Definitions");
	TextWrapped("Song	a collection of tracks, metadata, and other information that make up the "
				"project you are currently working on. Songs/Projects can be managed and created "
				"in the AXE Project Browser on launch.");
	TextWrapped("Track	a structure that groups clips together, represented on the Timeline's "
				"Y-axis. Track properties are shown on the left side of the Timeline and affect "
				"every clip on the Track. ");
	TextWrapped("Clip	a piece of audio that starts when the Scrubber passes over and stops upon "
				"the Scrubber's departure. Similar to the Track, Clips have properties that can be "
				"changed via its settings or effects.");
	TextWrapped("Automation	an effect with intensity controlled by a mathmatical function applied "
				"to a Track at any given time.");
	TextWrapped("Sample	a single unit of audio data, usually one IEEE 754 32-bit floating point "
				"number (f32).");
	TextWrapped("PCM Frame	a group of samples equal to the number of channels. For a stereo "
				"stream a frame is 2 samples, a mono frame is 1 sample, a 5.1 surround sound frame "
				"is 6 samples, etc.");
	TextWrapped("Channel	a stream of audio that is emitted from an individual speaker in a "
				"speaker system, or received from an individual microphone in a microphone system. "
				"A stereo stream has two channels (a left channel, and a right channel), a 5.1 "
				"surround sound system has 6 channels, etc.");
	TextWrapped("Sample Rate	the number of PCM frames that are processed per second. Always "
				"expressed in Hz, such as 44100, 48000, etc.");

	End();
}
}