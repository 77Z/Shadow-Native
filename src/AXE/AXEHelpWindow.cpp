#include "imgui.h"
#include "IconsCodicons.h"

namespace Shadow::AXE {
void updateHelpWindow(bool& p_open) {
	using namespace ImGui;

	if (!p_open)
		return;

	Begin("ShadowAudio Help", &p_open);

	SeparatorText("Basic Usage");

	TextWrapped("AXE is made up of various windows that you can use to get things done.");
	Bullet();
	TextWrapped("Use the arrow on the top left of windows to collapse them to take up less room, double-clicking the title bar also does this.");
	Bullet();
	TextWrapped("Many windows can be closed using the X button in the top right corner if you don't need them at the moment.");
	Bullet();
	TextWrapped("To open windows, the most common ones are listed in the toolbox, but every window can be found somewhere in the menu bar of the main window.");
	Bullet();
	TextWrapped("Click and drag the lower corner or the edges to resize windows");
	Dummy(ImVec2(2.0f, 5.0f));
	
	TextWrapped("You can move windows around your screen");
	Bullet();
	TextWrapped("Clicking and dragging in the window's blank space moves them");
	Bullet();
	TextWrapped("Clicking and dragging by the window's title bar lets you dock windows either to the Dockspace, or other windows");
	Dummy(ImVec2(2.0f, 5.0f));

	TextWrapped("Navigation");
	Bullet();
	TextWrapped("CTRL + CLICK on sliders or drag boxes to input a specific value");
	Bullet();
	TextWrapped("TAB / SHIFT + TAB to navigate with your keyboard");
	Bullet();
	TextWrapped("CTRL + TAB to cycle through open windows");
	Bullet();
	TextWrapped("Arrow keys to navigate");
	Bullet();
	TextWrapped("Space to activate");
	Bullet();
	TextWrapped("Enter to input text");
	Bullet();
	TextWrapped("Escape to move up and out");
	Bullet();
	TextWrapped("Enter to input text");
	Bullet();
	TextWrapped("While inputting text...");
	Indent();
	Bullet(); TextWrapped("CTRL + LEFT/RIGHT to jump words");
	Bullet(); TextWrapped("CTRL + A to select all");
	Bullet(); TextWrapped("CTRL + X/C/V to cut/copy/paste");
	Bullet(); TextWrapped("CTRL + Z/Y to undo/redo");
	Bullet(); TextWrapped("Escape to revert");
	Unindent();

	Separator();

	TextWrapped("The build of ShadowAudio included in this software currently supports the "
				"following backends:");

	TextUnformatted(ICON_CI_TERMINAL_POWERSHELL " Windows");
	BulletText("WASAPI");
	BulletText("DirectSound");
	BulletText("WinMM");

	TextUnformatted(ICON_CI_TERMINAL_LINUX " Linux");
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