#include "AXEDrumEngine.hpp"
#include "AXETypes.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "ShadowIcons.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "ppk_assert_impl.hpp"
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#define EC_THIS "Drum Engine"

// Forward Decls.
namespace Shadow::AXE {
bool DrumBeat(const char* label, bool* v);
}

namespace Shadow::AXE {

AXEDrumEngine::AXEDrumEngine(
	Song* songInfo,
	EditorState* editorState,
	ma_engine* audioEngine
)
	: songInfo(songInfo)
	, editorState(editorState)
	, audioEngine(audioEngine)
{
	EC_NEWCAT(EC_THIS);
}

AXEDrumEngine::~AXEDrumEngine() { }

static void checkRes(ma_result r) {
	if (r == MA_SUCCESS) return;

	EC_ERROUT(EC_THIS, "%s", ma_result_description(r));
	ERROUT("audio engine err: %s", ma_result_description(r));
}

void AXEDrumEngine::openDrumEditor(Clip* clip) {
	if (clip == nullptr) return;

	openedDrumCollections.emplace_back(clip);
}

void AXEDrumEngine::scheduleDrumPlayback(Clip* clip, uint64_t playbackTime) {
	if (clip == nullptr) {
		EC_WARN(EC_THIS, "Tried to play clip that doesn't exist, skipping!");
		return;
	}

	PPK_ASSERT(clip->clipType == TimelineClipType_Drums, "method user passed a non drum clip type?");

	// Calculate distance inbetween each beat. We can do this once up here
	// because it totally applies to every beat! Keep the logic in the nested 
	// for loops minimal!
	float secondsPerQuarterNote = 60.0f / songInfo->bpm;
	// Assuming the drum grid uses 16th note steps, so 4 steps per quarter note.
	float numStepsPerQuarterNote = 4.0f;
	float secondsPerStep = secondsPerQuarterNote / numStepsPerQuarterNote;
	float samplesPerStep = secondsPerStep * editorState->sampleRate;

	for (auto& drumTrack : clip->drumData->drumTracks) {
		fs::path samplePath = drumTrack.samplePath;

		EC_PRINT(EC_THIS, "Scheduliung drum track: %s with sample %s", samplePath.stem().string().c_str(), samplePath.string().c_str());
		
		for (size_t i = 0; i < drumTrack.beats.size(); i++) {
			if (drumTrack.beats[i] == false) continue;

			// TODO: This totally leaks memory! Can this be a smart pointer?
			ma_sound* beat = (ma_sound*)malloc(sizeof(ma_sound));

			// Push the pointer to an array and deallocate the memory when the
			// playhead stops. You can do ma_sound_stop on them before deleting
			// to prevent problems with playback

			ma_result res;
			
			res = ma_sound_init_from_file(
				audioEngine,
				samplePath.string().c_str(),
				MA_SOUND_FLAG_NO_SPATIALIZATION,
				&clip->drumData->drumSoundGroup,
				nullptr,
				beat
			);
			checkRes(res);

			ma_sound_seek_to_pcm_frame(beat, 0);
			// Ensure the frame time calculation results in uint64_t
			uint64_t scheduledFrame = (uint64_t)(samplesPerStep * i) + playbackTime;
			ma_sound_set_start_time_in_pcm_frames(beat, scheduledFrame);
			res = ma_sound_start(beat);
			checkRes(res);
		}
	}
}

void AXEDrumEngine::onUpdate() {
	using namespace ImGui;

	// Don't waste time doing anything else if there are no drums to edit
	if (openedDrumCollections.empty()) return;

	// currentlySelectedTrack->automations.erase(currentlySelectedTrack->automations.begin() + autoIt - 1)

	int it = 0;
	for (auto clip : openedDrumCollections) {
		
		auto removeMeFromOpened = [this, it]() {
			openedDrumCollections.erase(openedDrumCollections.begin() + it);
		};

		if (clip == nullptr) removeMeFromOpened();
		if (clip->clipType != TimelineClipType_Drums) removeMeFromOpened();

		// initialize drum data if it doesn't exist already
		if (clip->drumData == nullptr) clip->drumData = std::make_shared<DrumMachineData>();

		std::vector<DrumTrack>& drumTracks = clip->drumData->drumTracks;

		// resize if the number of measures doesn't match
		for (auto& dt : drumTracks) { // TODO: 4 is hardcoded here. Get it from the time sig instead!!
			if (dt.beats.size() / 4 != clip->drumData->measures) {
				dt.beats.resize(clip->drumData->measures * 4);
			}
		}

		std::string windowTitle =
			SHADOW_ICON_DRUMS " Drum Machine " SHADOW_ICON_ARROW_RIGHT + clip->name + "##" + std::to_string(it);

		PushStyleColor(ImGuiCol_Border, IM_COL32(255, 0, 0, 255));
		PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
		if (!Begin(windowTitle.c_str(), nullptr, ImGuiWindowFlags_MenuBar)) {
			PopStyleVar();
			PopStyleColor();
			End();
			it++;
			continue;
		}
		PopStyleVar();
		PopStyleColor();

		PushID(it);

		if (BeginMenuBar()) {

			SetNextItemWidth(30.0f);
			uint32_t minMeasures = 1;
			uint32_t maxMeasures = 127;
			if (DragScalar("Measures", ImGuiDataType_U32, &clip->drumData->measures, 0.5f, &minMeasures, &maxMeasures)) {
				// Recalculate clip length if measures changed
				if (songInfo->bpm > 0.0f && editorState->sampleRate > 0) {
					float secondsPerQuarterNote = 60.0f / songInfo->bpm;
					float totalSeconds = (float)clip->drumData->measures * secondsPerQuarterNote;
					clip->length = ( (uint64_t)(totalSeconds * editorState->sampleRate) ) / 100;
				} else {
					clip->length = 0; // Or handle as an error, BPM or sample rate is invalid
					if (songInfo->bpm <= 0.0f) {
						EC_WARN(EC_THIS, "BPM is zero or negative. Clip length calculation might be incorrect.");
					}
					if (editorState->sampleRate <= 0) {
						EC_WARN(EC_THIS, "Sample rate is zero or negative. Clip length calculation might be incorrect.");
					}
				}
			}

			if (!clip->drumData->drumTracks.empty()) {
				Text("%zu", clip->drumData->drumTracks.at(0).beats.size());
			} else {
				TextUnformatted("No drum tracks");
			}

			if (MenuItem("Close")) removeMeFromOpened();

			EndMenuBar();
		}

		auto winPos = GetWindowPos();
		auto winSize = GetWindowSize();
		auto fg = GetForegroundDrawList();
		auto mouse = GetMousePos();

		if (BeginDragDropTargetCustom(ImRect(winPos, winPos + winSize), GetID("DrumMachineDropTarget"))) {
			fg->AddCircleFilled(GetMousePos(), 5.0f, IM_COL32(0, 255, 0, 255));
			std::string text = "Add clip to drum machine " + clip->name;
			fg->AddText(ImVec2(mouse.x, mouse.y - 40.0f), IM_COL32(0, 255, 0, 255), text.c_str());

			if (const ImGuiPayload* payload = AcceptDragDropPayload("AXE_CLIP_PATH_PAYLOAD")) {
				// ma_result res;
				// ma_uint32 soundFlags = MA_SOUND_FLAG_NO_SPATIALIZATION;
				// if (songInfo->decodeOnLoad) soundFlags |= MA_SOUND_FLAG_DECODE;

				const char* clipPath = static_cast<const char*>(payload->Data);
				// std::string clipPathString(clipPath, clipPath + payload->DataSize);
				EC_PRINT("All", "dropped %s", clipPath);

				// Lesson in C++ for future Vince. The DrumTrack being created
				// here will die when it goes out of scope. But the reason it
				// doesn't have to be constructed as a shared_ptr is because it
				// will be copied by VALUE into the vector
				DrumTrack dt;
				dt.samplePath = fs::path(clipPath);
				drumTracks.emplace_back(dt);
			}

			EndDragDropTarget();
		}
		
		if (clip->drumData->drumTracks.empty()) {
			TextUnformatted("Drag drum samples into this window to add them to the track!");
		} else {
			// Drawing tracks
			ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInner;
			// if (BeginTable("drumModificationTable", clip->drumData->drumTracks.at(0).beats.size() + 1, tableFlags)) {

			int numBeats = static_cast<int>(clip->drumData->measures * 4);
			int cols = numBeats + 1;

			if (BeginTable("drumModificationTable", cols, tableFlags)) {
				TableSetupColumn("Sample");
				TableSetupScrollFreeze(1, 0);

				int trackIt = 0;
				for (auto& track : drumTracks) {
					PushID(trackIt);
					TableNextRow();
					TableSetColumnIndex(0);
					TextUnformatted(track.samplePath.stem().string().c_str());

					// Ensure the beats vector is large enough before accessing
					if (track.beats.size() < static_cast<size_t>(numBeats)) {
						track.beats.resize(numBeats, false);
					}
					for (int i = 0; i < numBeats; i++) {
						PushID(i);
						TableSetColumnIndex(i + 1);
						bool beatValue = track.beats[i];
						if (DrumBeat("", &beatValue)) {
							track.beats[i] = beatValue;
						}
						PopID();
					}
					PopID();
					trackIt++;
				}

				EndTable();
			}
		}

		#if 0
		{ // Stupid amount of code for the "done" button
			const char* doneButtonText = "Done";
			auto tsize = CalcTextSize(doneButtonText);
			SetCursorScreenPos(ImVec2(
				winPos.x + winSize.x - tsize.x - 50,
				winPos.y + winSize.y - tsize.y - 20
			));
			if (Button("Done")) removeMeFromOpened();
		}
		#endif

		PopID();

		End();

		it++;
	}
}

}