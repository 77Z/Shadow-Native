#define IMGUI_DEFINE_MATH_OPERATORS

#include "AXEDrumEngine.hpp"
#include "AXEJobSystem.hpp"
#include "AXENodeEditor.hpp"
#include "ShadowWindow.hpp"
#include "Configuration/EngineConfiguration.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <utility>
#include "AXETypes.hpp"
#include "AXETimeline.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <string>
#include "ShadowIcons.hpp"
#include "imgui/imgui_utils.hpp"
#include "Debug/EditorConsole.hpp"
#include "ppk_assert_impl.hpp"
#include <memory>
#include "ImGuiNotify.hpp"
#include "json_impl.hpp"
#include "imgui/imgui_api_patches.hpp"
#include "AXEAudioHelpers.hpp"

#define EC_THIS "Timeline"

namespace Shadow::AXE {

Timeline::Timeline(
	Song* songInfo,
	EditorState* editorState,
	ma_engine* audioEngine,
	ShadowWindow* window,
	AXENodeEditor* nodeEditor,
	AXEDrumEngine* drumEngine,
	PianoRoll* pianoRoll
)
	: songInfo(songInfo)
	, editorState(editorState)
	, audioEngine(audioEngine)
	, window(window)
	, nodeEditor(nodeEditor)
	, drumEngine(drumEngine)
	, pianoRoll(pianoRoll)
{
	EC_NEWCAT(EC_THIS);
	EC_PRINT(EC_THIS, "Scale factor from editorstate %.2f", editorState->sf);
}
Timeline::~Timeline() { }
#if 0
void Timeline::onUpdate() {
	using namespace ImGui;

	float sf = editorState->sf;

	playbackFrames = ma_engine_get_time_in_pcm_frames(audioEngine) / 100;

	if (!Begin("Timeline")) {
		End();
		return;
	}

	if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) &&
		IsKeyPressed(GetKeyIndex(ImGuiKey_Space)) &&
		!GetIO().WantTextInput)
		togglePlayback();

	// ImDrawList uses screen coords NOT window coords
	ImDrawList* drawList = GetWindowDrawList();
	ImVec2 canvasPos = GetCursorScreenPos();
	ImVec2 canvasSize = GetContentRegionAvail();
	ImVec2 canvasMax = GetWindowContentRegionMax();

	// For middle click drag
	ImVec2 mouseDelta = GetIO().MouseDelta;
	ImVec2 scrollDelta = ImVec2(-mouseDelta.x, -mouseDelta.y);

	if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Middle)) {
		SetScrollY(GetScrollY() + scrollDelta.y);
	}

	if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Right)) {
		ma_engine_set_time_in_pcm_frames(audioEngine,
			uint64_t(std::max(0.0f, GetMousePos().x - GetWindowPos().x - 390.0f) * 100));
	}

	SetCursorPosY(100.0f);

	int trackIt = 0;
	for (auto& track : songInfo->tracks) {
		PushID(trackIt);
		BeginChild("trackProps", ImVec2(250.0f * sf, 130.0f * sf));

		Text("%i", trackIt);
		PushItemWidth(200.0f * sf);
		InputText("##trackname", &track.name);
		SameLine();
		if (SmallButton(SHADOW_ICON_CLOSE)) songInfo->tracks.erase(songInfo->tracks.begin() + trackIt);
		SliderFloat("VOL", &track.volume, 0.0f, 100.0f);
		SliderFloat("BAL", &track.balence, -1.0f, 1.0f);

		Checkbox("Muted", &track.muted);

		EndChild();
		PopID();
		trackIt++;
	}

	if (Button("+ Add Track")) {
		Track newTrack;
		newTrack.name = "Untitled Track " + std::to_string(trackIt + 1);
		songInfo->tracks.push_back(newTrack);
	}

	SetCursorPos(ImVec2(260.0f * sf, 100.0f));

	trackIt = 0;
	BeginChild("timelineScroller", ImVec2(GetWindowWidth() - 270.0f, 2000.0f), false, ImGuiWindowFlags_AlwaysHorizontalScrollbar);

	if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Middle)) {
		SetScrollX(GetScrollX() + scrollDelta.x);
	}

	for (auto& track : songInfo->tracks) {
		PushID(trackIt);
		BeginChild("track", ImVec2(7000.0f * sf, 130.0f * sf));

		// Text("I am the data for track %s at index %i", track.name.c_str(), trackIt);

		// Clips render pipeline here
		int clipIt = 1; // NOT passed directly thorugh PushID as this causes potential overlap with track id's
		for (auto& clip : track.clips) {
			PushID(-1 * clipIt);
			PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 255));
			float clipWidth = (float)clip.length;
			SetCursorPos(ImVec2(clip.position, 2.0f));
			BeginChild("clip", ImVec2(clipWidth, GetWindowSize().y - 4.0f), 0, 0);

			if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Left)) {
				if (!clipBeingDragged) {
					clipBeingDragged = &clip;
					clipStoredMouseOffsetX = GetMousePos().x - GetWindowPos().x;
				}
			} else if (IsMouseReleased(ImGuiMouseButton_Left)) clipBeingDragged = nullptr;

			if (clipBeingDragged == &clip)
				clip.position = std::max(0.0f, clip.position + GetMousePos().x - GetWindowPos().x - clipStoredMouseOffsetX);


			SetNextItemWidth(clipWidth - 40.0f);
			InputText("##name", &clip.name);
			SameLine();
			if (Button(SHADOW_ICON_CLOSE)) track.clips.erase(track.clips.begin() + clipIt - 1);

			DragScalar("Length", ImGuiDataType_U64, &clip.length);
			Text("pos (u64) %lu | length (u64) %lu\npos + len (end rail pos) %lu",
				clip.position,
				clip.length,
				clip.position + clip.length);
			Text("Data src: %s", clip.baseAudioSource.c_str());

			EndChild();
			PopStyleColor();
			PopID();
			clipIt++;
		}

		EndChild();
		if (BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = AcceptDragDropPayload("AXE_CLIP_PATH_PAYLOAD")) {
				const char* clipPath = static_cast<const char*>(payload->Data);
				std::string clipPathString(clipPath, clipPath + payload->DataSize);

				Clip clip;
				clip.baseAudioSource = clipPathString;
				clip.name = "Untitled Clip";
				clip.length = 300;
				clip.position = static_cast<uint64_t>(GetMousePos().x - GetWindowPos().x);

				// load into mem
				ma_result result;

				ma_uint32 soundFlags = MA_SOUND_FLAG_NO_SPATIALIZATION;
				if (songInfo->decodeOnLoad) soundFlags |= MA_SOUND_FLAG_DECODE;
				ma_sound_init_from_file(audioEngine, clip.baseAudioSource.c_str(), soundFlags, nullptr, nullptr, &clip.engineSound);
				if (result != MA_SUCCESS) EC_ERROUT("Song Bootstrapper", "FAILED TO INIT SOUND FOR CLIP %s", clip.name.c_str());
				else clip.loaded = true;

				track.clips.push_back(clip);
			}
			EndDragDropTarget();
		}
		PopID();
		trackIt++;
	}

	EndChild();

	// Draw scrubber
	ImDrawList* fg = GetForegroundDrawList();
	SetCursorPosY(30.0f);
	PushItemWidth(400.0f);
	DragScalar("PCM Frames", ImGuiDataType_U64, &playbackFrames);
	SameLine();
	Text("Wants text input: %s", GetIO().WantTextInput ? "true" : "false");
	float scrubberWindowPos = (252 * sf) + playbackFrames - GetScrollX();
	fg->AddTriangleFilled(
		ImVec2(canvasPos.x + scrubberWindowPos - 10.0f, canvasPos.y + 30.0f),
		ImVec2(canvasPos.x + scrubberWindowPos + 10.0f, canvasPos.y + 30.0f),
		ImVec2(canvasPos.x + scrubberWindowPos + 1.0f, canvasPos.y + 63.0f),
		IM_COL32(255, 0, 0, 255)
	);
	fg->AddRectFilled(ImVec2(canvasPos.x + scrubberWindowPos, canvasPos.y + 60.0f), ImVec2(canvasPos.x + scrubberWindowPos + 2.0f, 10000.0f), IM_COL32(255, 0, 0, 255));

	// Scale / Ruler
	ImVec2 rulerOrigin = ImVec2(canvasPos.x + (252 * sf), canvasPos.y + 25.0f);
	float minorSize = 5.0f;
	float normal = 1.0f;
	drawList->AddLine(rulerOrigin, ImVec2(canvasPos.x + canvasMax.x, canvasPos.y + 25.0f), IM_COL32(255, 255, 255, 255), 2.0f);

	End();
}
#endif

static void loadClipDataFromAXEwf(std::shared_ptr<Clip> clip) {
	// TODO: This is repeat code from WaveformGen.cpp
	std::string globalLibraryPath = EngineConfiguration::getConfigDir() + "/AXEProjects/GlobalLibrary";
	std::string cachePath = EngineConfiguration::getConfigDir() + "/AXECachedData";
	std::string uniqueName = clip->baseAudioSource.substr(globalLibraryPath.length() + 1);
	std::replace(uniqueName.begin(), uniqueName.end(), '/', '-');

	std::string waveformFileLoc = cachePath + "/" + uniqueName + ".AXEwf";

	if (!std::filesystem::exists(waveformFileLoc)) {
		ImGui::InsertNotification({ImGuiToastType::Error, 5000, "Failed to load waveforms!"});
		return;
	}

	auto j = JSON::readBsonFile(waveformFileLoc);
	clip->waveformChannels = (int)j["channels"];

	for (auto& wavePoint : j["data"]) {
		clip->waveformData.emplace_back((int16_t)wavePoint);
	}
}

static float smoothstep(float edge0, float edge1, float x) {
	x = ImClamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return x * x * (3 - 2 * x);
}

static void drawBlinkingSquareAtCursor() {
	using namespace ImGui;
	auto currentCursor = GetCursorScreenPos();
	if (fmodf((float)ImGui::GetTime(), 0.40f) < 0.20f) { GetForegroundDrawList()->AddRectFilled(currentCursor, currentCursor + ImVec2(20,20), IM_COL32(255, 255, 0, 255)); }
}

void Timeline::onUpdate() {
	using namespace ImGui;

	if (singleSlicingClip) {
		window->setSECursor(ShadowEngineCursors_ClipSlicing);
	}

	float sf = editorState->sf;

	playbackFrames = ma_engine_get_time_in_pcm_frames(audioEngine) / 100;

	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	PushStyleVar(ImGuiStyleVar_ChildRounding, 7.0f);

	if (!Begin("Timeline")) {
		End();
		PopStyleVar(2);
		return;
	}
	PopStyleVar(2);

	ImGuiIO io = GetIO();

	if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) &&
		IsKeyPressed(GetKeyIndex(ImGuiKey_Space)) &&
		!io.WantTextInput)
		togglePlayback();

	if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) &&
		IsKeyPressed(ImGuiKey_B) &&
		io.KeyCtrl && !io.WantTextInput)
		newBookmark();

	// ImDrawList uses screen coords NOT window coords
	ImGuiWindow* timelineParentWindow = GetCurrentWindow();
	ImDrawList* drawList              = GetWindowDrawList();
	ImVec2 canvasPos                  = GetCursorScreenPos();
	ImVec2 canvasSize                 = GetContentRegionAvail();
	ImVec2 canvasMax                  = GetWindowContentRegionMax();

	drawList->AddText(editorState->headerFont, 40.0f, canvasPos + ImVec2(50, canvasSize.y - 45), IM_COL32(255, 255, 255, 100), "Timeline");

	// GetForegroundDrawList()->AddRect(canvasPos, canvasSize, IM_COL32(255, 255, 0, 255), 10.0f);
	// GetForegroundDrawList()->AddRect(canvasPos, canvasMax, IM_COL32(255, 255, 255, 255), 10.0f);

	// For middle click drag
	ImVec2 mouseDelta = GetIO().MouseDelta;
	ImVec2 scrollDelta = ImVec2(-mouseDelta.x, -mouseDelta.y);

	ImGuiTableFlags tblFlags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
	if (BeginTable("TimelineTable", 2, tblFlags)) {

		TableSetupScrollFreeze(1, 1);
		TableSetupColumn("Track Properties");
		TableSetupColumn("Timeline", ImGuiTableColumnFlags_NoHeaderLabel);
		TableHeadersRow();

		// Multi-select
		// ImGuiMultiSelectIO* msIo = BeginMultiSelect(ImGuiMultiSelectFlags_BoxSelect2d | ImGuiMultiSelectFlags_ClearOnEscape/* , clipSelection.Size */);
		// clipSelection.ApplyRequests(msIo);

		if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Left)) {
			if (selectionRect.Min == ImVec2(0, 0) && IsWindowHovered()) {
				selectionRect.Min = GetMousePos();
			}

			selectionRect.Max = GetMousePos();
		} else if (IsMouseReleased(ImGuiMouseButton_Left)) {
			selectionRect.Min = ImVec2(0, 0);
		}

		// Draw multi-select rect

		if (selectionRect.Min != ImVec2(0, 0)) {
			// Different rectangle bounds needed incase Min is greater than Max
			// in the selectionRect.
			ImRect postVisDrawRect;
			postVisDrawRect.Min = selectionRect.Min;
			postVisDrawRect.Max = selectionRect.Max;
			if (selectionRect.Min.x > selectionRect.Max.x) { postVisDrawRect.Max.x = selectionRect.Min.x; postVisDrawRect.Min.x = selectionRect.Max.x; }
			if (selectionRect.Min.y > selectionRect.Max.y) { postVisDrawRect.Max.y = selectionRect.Min.y; postVisDrawRect.Min.y = selectionRect.Max.y; }

			drawList->AddRectFilled(postVisDrawRect.Min, postVisDrawRect.Max, IM_COL32(255, 0, 0, 50), 6);
			drawList->AddRect(postVisDrawRect.Min, postVisDrawRect.Max, IM_COL32(255, 0, 0, 255), 6, 0, 3);
		}

		// GetForegroundDrawList()->AddText(canvasPos, IM_COL32(0, 255, 0, 255), std::string("stored mouse offset x: " + std::to_string(clipStoredMouseOffsetX)).c_str());

		int trackIt = 0;
		for (auto& track : songInfo->tracks) {
			PushID(trackIt);
			TableNextRow();
			TableSetColumnIndex(0);

			const ImVec2 cursorAtTopLeftOfTrackInfo = GetCursorScreenPos();
			
			if (ColorButton(
				"##TrackColor",
				track.color,
				ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip,
				ImVec2(233 * sf, 20)))
			{
				OpenPopup("TrackColorPop");
			}
			
			if (BeginPopup("TrackColorPop")) {
				ColorPicker3("Track Color", (float*)&track.color);
				EndPopup();
			}
			
			SetItemTooltip("Track Color (click to edit)");

			PushItemWidth(200.0f * sf);
			InputText("##TrackName", &track.name);
			SameLine();
			if (SmallButton(SHADOW_ICON_CLOSE)) songInfo->tracks.erase(songInfo->tracks.begin() + trackIt);
			if (SliderFloat("VOL", &track.volume, 0.0f, 100.0f, "%.0f")) {
				ma_sound_group_set_volume(&track.soundGroup, (track.volume / 100.0f));
			}
			if (SliderFloat("BAL", &track.balance, -1.0f, 1.0f, "%.2f")) {
				ma_sound_group_set_pan(&track.soundGroup, track.balance);
			}
			if (ToggleButton("M", &track.muted)) {
				if (track.muted) ma_sound_group_set_volume(&track.soundGroup, 0.0f);
			}
			SetItemTooltip("Mute Track");
			// ToggleButton("S", )

			SameLine();
			if (Button(SHADOW_ICON_ROBOT)) {
				currentlySelectedTrack = &track;
				OpenPopup("Track Automations");
			}
			SetItemTooltip("Track Automations");
			updateTrackAutomationsPopup();

			// SameLine();
			// if (Button("Use Nodes")) {
			// 	JobSystem::degradeEditorWithMessage("Test throw", "What in the heck is going on");
			// }

			// the poor mans toggle button
			SameLine();
			if (track.realPanning && Button("PAN##panButton")) {
				track.realPanning = false;
			} else if (!track.realPanning && Button("BAL##balanceButton")) {
				track.realPanning = true;
			}

			SameLine();
			if (Button(SHADOW_ICON_DRUMS)) {
				newDrumCollection(&track, playbackFrames);
			}
			SetItemTooltip("[Drum Engine] new drum collection at play head");

			SameLine();
			if (Button(SHADOW_ICON_PIANO)) {
				newPianoRoll(&track, playbackFrames);
			}
			SetItemTooltip("[Piano Roll] new piano roll at play head");

			const ImVec2 cursorAtBottomOfTrackInfo = GetCursorScreenPos();

			const float trackHeight = cursorAtBottomOfTrackInfo.y - cursorAtTopLeftOfTrackInfo.y;

			TableSetColumnIndex(1);

			// TODO: This isn't needed anymore right?
			PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0,0,0,0));

			// top left of the cell
			ImVec2 screenPosOrigin = GetCursorScreenPos();

			auto tableDrawList = GetCurrentTable()->InnerWindow->DrawList;

			PushClipRect(timelineParentWindow->ClipRect.Min, timelineParentWindow->ClipRect.Max, false);
			// Track background color tint
			tableDrawList->AddRectFilled(
				ImVec2(screenPosOrigin.x + GetScrollX(), screenPosOrigin.y),
				ImVec2(screenPosOrigin.x + GetWindowWidth() + GetScrollX(), screenPosOrigin.y + trackHeight),
				ImColor(track.color.Value.x, track.color.Value.y, track.color.Value.z, 0.1f)
			);
			PopClipRect();

			// TODO: This placement is all messed up :(
			{ // Line indicating the length of the song.
				float endLinePos = float(songInfo->songLength) * (editorState->zoom / 100.0f);
				tableDrawList->AddLine(ImVec2(endLinePos, canvasPos.y), ImVec2(endLinePos, canvasPos.y + canvasSize.y), IM_COL32(0, 255, 255, 255));
			}

			int clipIt = 0;
			// float left = GetCursorPosX();

			// At any point during the clip rendering it can intercept this and
			// set it to false when a clip shout NOT be deselected. (i.e. on
			// click and subsequent selection of a clip)
			bool cancelDeselection = false;

			for (auto& clip : track.clips) {
				PushID(clipIt);

				ImGuiID id = GetID(clipIt);

				float clipPosition = (float)clip->position * (editorState->zoom / 100.0f);
				float clipWidth = (float)clip->length * (editorState->zoom / 100.0f);

				// DEBUG BS
				if (editorState->timelinePositionDebugMode && fmodf((float)ImGui::GetTime(), 0.40f) < 0.20f) { tableDrawList->AddRectFilled(screenPosOrigin, screenPosOrigin + ImVec2(20,20), IM_COL32(0, 255, 0, 255)); }

				ImRect bounds = ImRect(
					screenPosOrigin + ImVec2(clipPosition, 7),
					screenPosOrigin + ImVec2(clipPosition + clipWidth, trackHeight - 7));

				// Button behavior doesn't work here :P
				// checking manual hovers is the way some ImGui native
				// components do it, so I don't see the problem with doing it.
				bool clipHovered = ItemHoverable(ImRect(
					// hoverable hitbox needs special bounds specifically to
					// exclude the left and right crop grabbers because the
					// imgui logic doesn't like overlapping ones. There is an
					// allow overlap flag that might work here but it's not
					// worth the simple bounds calculation honestly.
					ImVec2(bounds.Min.x + 10, bounds.Min.y),
					ImVec2(bounds.Max.x - 10, bounds.Max.y)
				), id, 0);

				// This can't be IsMouseClicked because the hit detection would
				// select the clip before the user gets a chance to drag it and
				// potentially others
				if (clipHovered && IsMouseReleased(ImGuiMouseButton_Left)) {
					if (!GetIO().KeyCtrl) selectedClips.clear();
					selectedClips.emplace_back(clip.get());
					cancelDeselection = true;
				}

				// Am I a clip that's selected? Highlight myself if so.
				for (auto& selectedClip : selectedClips) {
					if (selectedClip == clip.get()) {
						tableDrawList->AddRect(bounds.Min, bounds.Max, IM_COL32(255, 255, 255, 255), 3.0f, 0, 5.0f);
					}
				}

				// Double click a clip to invoke action upon it
				if (clipHovered && IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					if (clip->clipType == TimelineClipType_Drums) {
						drumEngine->openDrumEditor(clip.get());
					} else if (clip->clipType == TimelineClipType_PianoRoll) {
						pianoRoll->openPianoRoll(clip.get());
					}
				}

				{ // Clip cropping
					auto fg = GetForegroundDrawList();

					ImRect leftCropGrabber = ImRect(bounds.Min, ImVec2(bounds.Min.x + 10, bounds.Max.y));
					ImRect rightCropGrabber = ImRect(ImVec2(bounds.Max.x - 10, bounds.Min.y), bounds.Max);

					bool leftCropHovered = ItemHoverable(leftCropGrabber, GetID(/* clipIt + 1 */34543), 0);
					bool rightCropHovered = ItemHoverable(rightCropGrabber, GetID(clipIt + 2), 0);

					static bool leftCurrentlyCropping = false; // static, shared across all clips
					static bool rightCurrentlyCropping = false;

					// if (leftCropHovered) window->setSECursor(ShadowEngineCursors_CropClipLeft);
					// if (rightCropHovered) window->setSECursor(ShadowEngineCursors_CropClipRight);

					if (leftCropHovered) {
						window->setSECursor(ShadowEngineCursors_CropClipLeft);
						fg->AddRectFilled(leftCropGrabber.Min, leftCropGrabber.Max, IM_COL32(255, 0, 0, 255));

						if (IsMouseDown(ImGuiMouseButton_Left)) {
							selectedClips.clear();
							selectedClips.emplace_back(clip.get());

							leftCurrentlyCropping = true;
						} else {
							leftCurrentlyCropping = false;
						}
					}

					if (rightCropHovered) {
						fg->AddRectFilled(rightCropGrabber.Min, rightCropGrabber.Max, IM_COL32(255, 0, 0, 255));


						if (IsMouseDown(ImGuiMouseButton_Left)) {
							selectedClips.clear();
							selectedClips.emplace_back(clip.get());

							rightCurrentlyCropping = true;
						} else {
							rightCurrentlyCropping = false;
						}
					}

					// GetForegroundDrawList()->AddRectFilled(leftCropGrabber.Min, leftCropGrabber.Max, IM_COL32(0, 0, 255, 255));
					// GetForegroundDrawList()->AddRectFilled(rightCropGrabber.Min, rightCropGrabber.Max, IM_COL32(0, 0, 255, 255));
				}

				{ // Clip slicing
					if (clipHovered && singleSlicingClip) {
						auto fg = GetForegroundDrawList();
						auto mousePos = GetMousePos();

						fg->AddText(ImVec2(mousePos.x + 20, bounds.Min.y + 40), IM_COL32(255, 255, 255, 255), "Click to slice...");
						fg->AddLine(ImVec2(mousePos.x, bounds.Min.y), ImVec2(mousePos.x, bounds.Max.y), IM_COL32(255, 255, 255, 255), 3.0f);

						if (IsMouseClicked(ImGuiMouseButton_Left)) {
							// perform slice
						}
					}
				}


				// This is the mouse offset diff
				// GetForegroundDrawList()->AddLine(bounds.Min, ImVec2(GetMousePos().x, bounds.Min.y), IM_COL32(0, 255, 0, 255), 3);
				// GetForegroundDrawList()->AddText(bounds.Min + ImVec2(10, 15), IM_COL32(0, 255, 0, 255), std::string(std::to_string(
				// 	GetMousePos().x - bounds.Min.x
				// )).c_str());


				// // This is terrible code bro.
				// bool actionClipsBeingDragged_thisFrame = actionClipsBeingDragged;
				// if (clipHovered && IsMouseDragging(ImGuiMouseButton_Left)) {
				// 	actionClipsBeingDragged = true;
				// }

				// if (actionClipsBeingDragged_thisFrame != actionClipsBeingDragged && actionClipsBeingDragged) {
				// 	EC_PRINT(EC_THIS, "USER STARTED DRAG ACTION");
				// 	startClipDragging();
				// }



				// if (actionClipsBeingDragged && IsMouseReleased(ImGuiMouseButton_Left)) {
				// 	actionClipsBeingDragged = false;
				// 	EC_PRINT(EC_THIS, "User is NOT DRAGGING CLIPS ANYMORE");
				// }

				// Clip drawing
				tableDrawList->AddRectFilled(bounds.Min, bounds.Max, ImColor(track.color.Value.x, track.color.Value.y, track.color.Value.z, actionClipsBeingDragged ? 0.1f : 0.8f), 3.0f);
				tableDrawList->AddText(bounds.Min, IM_COL32(255, 255, 255, 255), clip->name.c_str());
				if (clipHovered) {
					auto volText = "0dB";
					auto textSize = CalcTextSize(volText);
					auto bottomLeftVolTextPos = ImVec2(bounds.Min.x, bounds.Max.y - textSize.y);
					tableDrawList->AddRectFilled(bottomLeftVolTextPos, bottomLeftVolTextPos + textSize, IM_COL32(0, 0, 0, 200));
					tableDrawList->AddText(bottomLeftVolTextPos, IM_COL32(255, 255, 255, 255), volText);
				}

				if (editorState->timelinePositionDebugMode) {
					tableDrawList->AddRectFilled(bounds.Min, ImVec2(bounds.Max.x - 100, bounds.Max.y), IM_COL32(0, 0, 0, 140));
					tableDrawList->AddText(bounds.Min, IM_COL32(255, 255, 255, 255), std::string("I am \"" + clip->name + "\"\nat data pos: " + std::to_string(clip->position) + "\ndata len: " + std::to_string(clip->length)).c_str());
				}

				// -----------------------------------------------------------//
				// FIXME: I HATE THIS DRAGGING CODE SO SO SO MUCH AHHHHHHH
				// -----------------------------------------------------------//

				// Am I a clip that is currently being dragged?
				bool currentClipBeingDragged = false;
				for (auto& draggingClip : clipsBeingDragged)
					if (draggingClip.first == clip.get()) {
						currentClipBeingDragged = true;

						EC_PRINT(EC_THIS, "Initial position! %lu", draggingClip.second);
						clip->position = std::max(0.0f, draggingClip.second + GetMousePos().x - GetWindowPos().x - clipStoredMouseOffsetX);
					}
				
				if (!currentClipBeingDragged) {
					if (clipHovered && IsMouseDragging(ImGuiMouseButton_Left) /* !currentClipBeingDragged is implied in this block */) {
						// flush all selected clips to being dragged state
						// clipBeingDragged = clip.get();

						for (auto& selectedClip : selectedClips) clipsBeingDragged.emplace_back(std::pair<Clip*, uint64_t>(selectedClip, selectedClip->position));

						clipStoredMouseOffsetX = GetMousePos().x - GetWindowPos().x;
						// clipStoredMouseOffsetX = GetMousePos().x - clip->position; // AI GEN that works but stores current clip pos in global var
					}
				}
				if (IsMouseReleased(ImGuiMouseButton_Left)) {
					// EC_PRINT(EC_THIS, "RELEASED!!");
					clipsBeingDragged.clear();
				}

				#if 0
				// Bruh

				bool currentClipBeingDragged = false;
				float* clipStoredMouseOffsetX = nullptr;
				for (auto& draggingClip : clipsBeingDragged)
					if (draggingClip.first == clip.get()) {
						currentClipBeingDragged = true;
						clipStoredMouseOffsetX = &draggingClip.second;
						break;
					}

				
				if (currentClipBeingDragged) {
					clip->position = GetMousePos().x - *clipStoredMouseOffsetX;
				} else {
					if (clipHovered && IsMouseDown(ImGuiMouseButton_Left) /* !currentClipBeingDragged is implied in this block */) {
						// flush all selected clips to being dragged state
						// clipBeingDragged = clip.get();

						for (auto& selectedClip : selectedClips) clipsBeingDragged.emplace_back(std::pair<Clip*, float>(selectedClip, 0.0f));

						// clipStoredMouseOffsetX = GetMousePos().x - GetWindowPos().x;
						for (auto& draggingClip : clipsBeingDragged) {
							if (draggingClip.first == clip.get()) {
								draggingClip.second = GetMousePos().x - clip->position;
								break;
							}
						}
					}
				}
				if (IsMouseReleased(ImGuiMouseButton_Left)) {
					EC_PRINT(EC_THIS, "RELEASED!!");
					clipsBeingDragged.clear();
				}

				//old notes
				if (clipHovered && IsMouseDown(ImGuiMouseButton_Left) && !clipBeingDragged) {
					clipBeingDragged = clip.get();
					clipStoredMouseOffsetX = GetMousePos().x - clip->position;
				} else if (IsMouseReleased(ImGuiMouseButton_Left)) clipBeingDragged = nullptr;

				if (clipBeingDragged == clip.get()) {
					// Actively dragging
					clip->position = GetMousePos().x - clipStoredMouseOffsetX;
				}
				#endif



				PopID();
				SameLine();
				clipIt++;
			}

			// If the user clicks and it isn't on a clip it should be deselected.
			// FIXME: Doesn't really work with multi-select... or dragging...
			// if (IsWindowHovered() && IsMouseClicked(ImGuiMouseButton_Left) && !cancelDeselection)
			// 	selectedClips.clear();

			// Old clip rendering function
			#if 0
			int clipIt = 0;
			float left = GetCursorPosX();
			for (auto& clip : track.clips) {
				PushID(clipIt);
				SetCursorPosX(left + (float)clip->position * (editorState->zoom / 100.0f));
				float clipWidth = (float)clip->length * (editorState->zoom / 100.0f);
				BeginChild("Clip", ImVec2(clipWidth, 143.0f), 0, 0);

				if (selectedClip != nullptr && selectedClip == clip.get()) {
					GetWindowDrawList()->AddRect(
						ImVec2(GetCursorScreenPos()),
						ImVec2(GetCursorScreenPos().x + clipWidth, GetCursorScreenPos().y + 143),
						IM_COL32(255, 255, 255, 255),
						5.0f,
						0,
						4.0f
					);
				}
				if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseClicked(ImGuiMouseButton_Left)) selectedClip = clip.get();

				// Clip dragging logic
				if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Left) && !clipBeingDragged) {
					clipBeingDragged = clip.get();
					clipStoredMouseOffsetX = GetMousePos().x - GetWindowPos().x;
				} else if (IsMouseReleased(ImGuiMouseButton_Left)) clipBeingDragged = nullptr;

				if (clipBeingDragged == clip.get()) {
					// Actively dragging
					clip->position = std::max(0.0f, clip->position + GetMousePos().x - GetWindowPos().x - clipStoredMouseOffsetX);
					#if 0 //TODO: this is bugged :P
					if (!IsWindowHovered()) {
						int n = GetMouseDragDelta(0).y < 0.0f ? -1 : 1;
						EC_PRINT(EC_THIS, "Clip swap %i", n);
						songInfo->tracks.at(trackIt + n).clips.push_back(clip);
						track.clips.erase(track.clips.begin() + clipIt + 1);
					}
					#endif
				}

				SetNextItemWidth(clipWidth - (editorState->zoom > 70 ? 55.0f : 0.0f));
				InputText("##ClipName", &clip->name);
				if (editorState->zoom > 70) {
					SameLine();
					if (SmallButton(SHADOW_ICON_CLOSE)) track.clips.erase(track.clips.begin() + clipIt + 1);
					if (IsItemHovered()) window->setSECursor(ShadowEngineCursors_CropClipRight);
				}
				// if (SmallButton(SHADOW_ICON_CHROME_RESTORE)) {
					// clip->shouldDrawWaveform =! clip->shouldDrawWaveform;

					//TODO: move this to more static code
					if (clip->shouldDrawWaveform && clip->waveformData.empty()) {
						EC_PRINT(EC_THIS, "Clip waveform data empty! Loading now");
						loadClipDataFromAXEwf(clip);
					}
				// }
				if (IsWindowHovered() && BeginTooltip()) {
					Text("pos (u64) %lu\nlength (u64) %llu\npos + len (end rail pos) %llu",
						clip->position,
						clip->length,
						clip->position + clip->length);
					EndTooltip();
				}

				if (clip->shouldDrawWaveform && !clip->waveformData.empty()) {
					ImDrawList* wdl = GetWindowDrawList();
					ImVec2 basePos = GetCursorScreenPos();
					// debug rect
					// wdl->AddRect(ImVec2(basePos), basePos + GetContentRegionAvail(), IM_COL32(0, 255, 0, 255));

					//Draw baselines
					// There are L and R channels here
					auto halfwayY = basePos.y + (GetContentRegionAvail().y / 2);
					wdl->AddLine(ImVec2(basePos.x, halfwayY), ImVec2(basePos.x + GetContentRegionAvail().x, halfwayY), IM_COL32(255, 0, 0, 255), 3);

					// GetForegroundDrawList()->AddRect(GetMainViewport()->Pos, GetMainViewport()->Size, IM_COL32(0, 255, 0, 255));
					// PushClipRect(GetMainViewport()->Pos, GetMainViewport()->Size, false);

					if (clip->waveformChannels == 1) {
						for (size_t i = 0; i < clip->waveformData.size(); i++) {
							float x = basePos.x + (i * editorState->zoom / 100);
							wdl->AddLine(ImVec2(x, halfwayY + clip->waveformData.at(i) * 0.003), ImVec2(x, halfwayY), IM_COL32(255, 0, 0, 255));
						}
					}

					// PopClipRect();
				}
				EndChild();
				PopID();
				SameLine();
				clipIt++;
			}
			#endif
			SetCursorPosX((float)songInfo->songLength);
			InvisibleButton("##SameLineStopper", ImVec2(1,1));

			PopStyleColor();

			// Track automations
			int autoit = 1;
			for (auto& automation : track.automations) {

				if (!automation.visible) continue;

				PushID(autoit);

				auto cursorAtAutomationStart = GetCursorScreenPos();

				ImColor col = automation.color;
				ImRect bounds = ImRect(screenPosOrigin.x, screenPosOrigin.y, GetWindowPos().x + GetWindowWidth(), screenPosOrigin.y + trackHeight);

				ImDrawList* fg = GetForegroundDrawList();

				if (editorState->automationDebugMode) {
					fg->AddRect(bounds.Min, bounds.Max, col, 2.0f, 0, 2);
					fg->AddText(bounds.Min + ImVec2(10,10), col, "AUTOMATION DEBUG");
				}

				PushClipRect(bounds.Min, bounds.Max, false);

				/* { // Start Rail
					float startRailPos = (bounds.Min.x + (float)automation.startRail) * editorState->zoom / 100;
					drawList->AddTriangleFilled(
						ImVec2(startRailPos - 15, bounds.Min.y),
						ImVec2(startRailPos, bounds.Min.y),
						ImVec2(startRailPos, bounds.Min.y + 15),
						col
					);
					drawList->AddRectFilled(
						ImVec2(startRailPos, bounds.Min.y),
						ImVec2(startRailPos + 5, bounds.Max.y),
						col
					);
					SetCursorScreenPos(ImVec2(startRailPos - 15, bounds.Min.y));
					InvisibleButton("##StartRailGrabber", ImVec2(20, bounds.Max.y - bounds.Min.y));
					if (IsItemHovered() && IsMouseDown(0) && !automationStartRailBeingDragged) {
						automationStartRailBeingDragged = &automation;
					} else if (IsMouseReleased(0)) automationStartRailBeingDragged = nullptr;
					SetItemTooltip("Automation Start Rail");
					if (automationStartRailBeingDragged == &automation) {
						// Actively dragging
						automation.startRail = std::max(0.0f, GetMousePos().x - GetWindowPos().x - 376.0f);
						if (automation.startRail > automation.endRail - 20) automation.startRail = automation.endRail - 20;
					}
				}

				{ // End Rail
					float endRailPos = (bounds.Min.x + (float)automation.endRail) * editorState->zoom / 100;
					drawList->AddTriangleFilled(
						ImVec2(endRailPos + 20, bounds.Min.y),
						ImVec2(endRailPos + 5, bounds.Min.y),
						ImVec2(endRailPos + 5, bounds.Min.y + 15),
						col
					);
					drawList->AddRectFilled(
						ImVec2(endRailPos, bounds.Min.y),
						ImVec2(endRailPos + 5, bounds.Max.y),
						col
					);

					SetCursorScreenPos(ImVec2(endRailPos, bounds.Min.y));
					InvisibleButton("##EndRailGrabber", ImVec2(20, bounds.Max.y - bounds.Min.y));
					if (IsItemHovered() && IsMouseDown(0) && !automationEndRailBeingDragged) {
						automationEndRailBeingDragged = &automation;
					} else if (IsMouseReleased(0)) automationEndRailBeingDragged = nullptr;
					SetItemTooltip("Automation End Rail");
					if (automationEndRailBeingDragged == &automation) {
						// Actively dragging
						automation.endRail = std::max(0.0f, GetMousePos().x - GetWindowPos().x - 376.0f);
						if (automation.endRail < automation.startRail + 20) automation.endRail = automation.startRail + 20;
					}
				} */

				size_t pointit = 1;
				for (auto& point : automation.points) {
					PushID(pointit);
					uint64_t x = point.first;
					float y = point.second;
					auto pointVerticalRange = trackHeight - 10.0f;
					auto pixelDrawingRange = (1.0f - y) * pointVerticalRange; // Casts 0.0f-1.0f range to 0-trackheight range;

					SetCursorScreenPos(ImVec2(bounds.Min.x + x - 10, bounds.Min.y + pixelDrawingRange - 10));
					InvisibleButton("PointGrabber", ImVec2(20,20));
					// TODO: add logic to reject if a clip is being dragged
					if (IsItemHovered() && IsMouseDown(0) && !autoPointBeingDragged) {
						autoPointBeingDragged = GetItemID();
					} else if (IsMouseReleased(0)) autoPointBeingDragged = 0;
					if (autoPointBeingDragged == GetItemID()) {
						// Actively dragging
						ImVec2 mousePos = GetMousePos(), winPos = GetWindowPos();
						point.first = mousePos.x - winPos.x - 376.0f;

						// The vertical position of the point needs to be converted to a 0.0f-1.0f range

						// The point can't go all the way to the bottom of the track. This value is in pixels,
						// not the percent that we need for storage.

						if (mousePos.y > cursorAtAutomationStart.y && mousePos.y < (cursorAtAutomationStart.y + trackHeight - 10.0f)) {
							float range = (mousePos.y - cursorAtAutomationStart.y) / (pointVerticalRange);
							range = ImClamp(range, 0.0f, 1.0f); // not really needed, just for safety.
							range = 1.0f - range;
							if (range < 0.03f) range = 0.0f;
							if (range > 0.97f) range = 1.0f;
							point.second = range;
						}

						SetTooltip("%.1f%%", point.second * 100);
					}

					// Point drawing here
					drawList->AddCircleFilled(
						ImVec2(bounds.Min.x + x, bounds.Min.y + pixelDrawingRange),
						10.0f,
						col,
						4
					);

					drawList->AddCircleFilled(
						ImVec2(bounds.Min.x + x, bounds.Min.y + pixelDrawingRange),
						3.0f,
						IM_COL32(0, 0, 0, 255),
						4
					);



					// Draw curve
					if (automation.points.size() == pointit) {
						// No next point
					} else {
						auto& nextPoint = automation.points.at(pointit);
						// Screen point locations
						const ImVec2 p1 = ImVec2(bounds.Min.x + point.first, bounds.Min.y + pixelDrawingRange);
						const ImVec2 p2 = ImVec2(bounds.Min.x + nextPoint.first, bounds.Min.y + (1.0f - nextPoint.second) * (trackHeight - 10.0f));

						size_t subStepCount = automation.smoothCurve ? 20 : 2;
						float step = 1.f / float(subStepCount - 1);
						for (size_t substep = 0; substep < subStepCount - 1; substep++) {
							float t = float(substep) * step;

							const ImVec2 sp1 = ImLerp(p1, p2, t);
							const ImVec2 sp2 = ImLerp(p1, p2, t + step);

							const float rt1 = smoothstep(p1.x, p2.x, sp1.x);
							const float rt2 = smoothstep(p1.x, p2.x, sp2.x);

							const ImVec2 pos1 = ImVec2(sp1.x, ImLerp(p1.y, p2.y, rt1))/*  * viewSize + offset */;
							const ImVec2 pos2 = ImVec2(sp2.x, ImLerp(p1.y, p2.y, rt2))/*  * viewSize + offset */;

							drawList->AddLine(
								pos1,
								pos2,
								col, 2.0f
							);
						}
					}

					PopID();
					pointit++;
				}

				PopClipRect();
				PopID();
				autoit++;
			}

			PopID();
			trackIt++;
		}

#if 0
		TableNextRow();
		TableSetColumnIndex(0);
		if (Button(SHADOW_ICON_PLUS " Add Track")) {
			Track newTrack;
			newTrack.name = "Untitled Track " + std::to_string(trackIt + 1);
			songInfo->tracks.push_back(newTrack);
		}
#endif

		// GetForegroundDrawList()->AddText(ImVec2(900, 700), IM_COL32(255, 255, 255, 255), std::to_string(TableGetHoveredRow()).c_str());
		tableHoveredRow = TableGetHoveredRow();

		drawBlinkingSquareAtCursor();

		if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Middle)) {
			if (GetIO().KeyCtrl) {
				editorState->zoom -= scrollDelta.x;
				SetScrollX(GetScrollX() - scrollDelta.x);
				if (editorState->zoom < 10) editorState->zoom = 10;
			} else {
				SetScrollX(GetScrollX() + scrollDelta.x);
				SetScrollY(GetScrollY() + scrollDelta.y);
			}
		}

		// Timeline scrubbing set on right click. (still needs to snap to clips)
		if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Right)) {
			// At the time that this calculation is being done, the X pos of the
			// screen cursor is at 0px on the timeline.

			// The procedure for setting the scrubber position correctly is:
			// 1. subtract screen cursor x from mouse position x to get relative
			// pixel offset on timeline.
			float relativePixelOffset = GetMousePos().x - GetCursorScreenPos().x;

			// 2. take the x offset and multiply by 100 to gwt thw amount of PCM
			// frames. (1 pcm frame = 100px visually at 1x zoom).
			// 3. divide that by (zoom factor / 100.0f) to 1x the scale.
			float pcmFramesToBeSet = (relativePixelOffset * 100) / (editorState->zoom / 100.0f);

			ma_engine_set_time_in_pcm_frames(audioEngine, uint64_t(std::max(0.0f, pcmFramesToBeSet)));
		}

		PushClipRect(
			ImVec2(canvasPos.x + (245*sf), canvasPos.y),
			ImVec2(canvasPos.x + canvasMax.x, canvasPos.y + canvasMax.y + 1000.0f),
			false
		);

		// ImDrawList* tableDrawList = GetWindowDrawList();
		ImDrawList* tableDrawList = GetCurrentTable()->InnerWindow->DrawList;

		ImVec2 currentCursorPos = GetCursorScreenPos();

		if (fmodf((float)ImGui::GetTime(), 0.40f) < 0.20f) { tableDrawList->AddRectFilled(currentCursorPos, currentCursorPos + ImVec2(20,20), IM_COL32(0, 255, 0, 255)); }
		tableDrawList->AddText(currentCursorPos, IM_COL32(255, 255, 255, 255), std::to_string(sf).c_str());

		// Draw Scrubber
		// Testing a new way to draw the scrubber
		auto newScrubberXPosition = currentCursorPos.x + (playbackFrames * (editorState->zoom / 100.0f));
		tableDrawList->AddRectFilled(
			// MIN RECT (takes current x and uses top of drawable area for y)
			ImVec2(newScrubberXPosition, canvasPos.y),
			// MAX RECT
			ImVec2(newScrubberXPosition + 2.0f, GetWindowPos().y + GetWindowHeight()),
			IM_COL32(0, 255, 0, 255));

		// Draw Scale / Ruler
		ImColor textColor = GetStyle().Colors[ImGuiCol_Text];

		float majorUnit       = 100.0f * (editorState->zoom / 100.0f);
		float minorUnit       = 10.0f * (editorState->zoom / 100.0f);
		float tinyUnit        = 1.0f * (editorState->zoom / 100.0f);

		float labelAlignment  = 0.6f;
		float sign            = 1.0f;
		float labelDistance   = 8.0f;

		// Vertical heights in timeline top bar
		float tinySize        = 10.0f;
		float minorSize       = 15.0f;
		float majorSize       = 30.0f;

		auto currentCursor = GetCursorScreenPos();
		drawBlinkingSquareAtCursor();
		ImVec2 from = ImVec2(currentCursor.x, canvasPos.y);
		ImVec2 to = ImVec2(canvasPos.x + canvasMax.x, canvasPos.y);

		ImVec2 direction = (to - from) * ImInvLength(to - from, 0.0f);
		ImVec2 normal    = ImVec2(direction.y, direction.x);
		float distance   = sqrtf(ImLengthSqr(to - from));

		tableDrawList->AddLine(from, to, textColor);

		auto p = from;

		if (editorState->zoom > 400.0f)
			for (float d = 0.0f; d <= distance; d += tinyUnit, p += direction * tinyUnit)
				tableDrawList->AddLine(p - normal * tinySize, p + normal * tinySize, textColor);

		if (editorState->zoom > 70.0f)
			for (float d = 0.0f; d <= distance; d += minorUnit, p += direction * minorUnit)
				tableDrawList->AddLine(p - normal * minorSize, p + normal * minorSize, textColor);

		if (editorState->zoom > 20.0f)
			for (float d = 0.0f; d <= distance + majorUnit; d += majorUnit) {
				p = from + direction * d;

				tableDrawList->AddLine(p - normal * majorSize, p + normal * majorSize, textColor);
				tableDrawList->AddLine(p - normal * canvasMax.y, p + normal * canvasMax.y, ImColor(textColor.Value.x, textColor.Value.y, textColor.Value.z, 0.3f));

				if (d == 0.0f)
					continue;

				// if (editorState->zoom < 50.0f) continue;

				// char label[16];
				// snprintf(label, 15, "%g", d * sign);
				// ImVec2 labelSize = ImGui::CalcTextSize(label);

				// ImVec2 labelPosition = p + ImVec2(fabsf(normal.x), fabsf(normal.y)) * labelDistance;
				// float labelAlignedSize = ImDot(labelSize, direction);
				// labelPosition += direction * (-labelAlignedSize + labelAlignment * labelAlignedSize * 2.0f);
				// labelPosition = ImFloor(labelPosition + ImVec2(0.5f, 0.5f));

				// tableDrawList->AddText(labelPosition, textColor, label);
			}

		PopClipRect();

		// msIo = EndMultiSelect();
		EndTable();
	}

	if (BeginDragDropTargetCustom(ImRect(GetWindowPos(), GetWindowPos() + GetWindowSize()), GetID("TimelineDropTarget"))) {

		ImDrawList* fg = GetForegroundDrawList();
		ImVec2 mouse = GetMousePos();

		if (tableHoveredRow != -1 && tableHoveredRow != 0) {
			fg->AddCircleFilled(GetMousePos(), 5.0f, IM_COL32(0, 255, 0, 255));
			std::string text = "Add clip to track " + std::to_string(tableHoveredRow);
			fg->AddText(ImVec2(mouse.x, mouse.y - 40.0f), IM_COL32(0, 255, 0, 255), text.c_str());

			if (const ImGuiPayload* payload = AcceptDragDropPayload("AXE_CLIP_PATH_PAYLOAD")) {
				ma_result res;
				ma_uint32 soundFlags = MA_SOUND_FLAG_NO_SPATIALIZATION;
				if (songInfo->decodeOnLoad) soundFlags |= MA_SOUND_FLAG_DECODE;

				const char* clipPath = static_cast<const char*>(payload->Data);
				std::string clipPathString(clipPath, clipPath + payload->DataSize);


				std::shared_ptr<Clip> clip = std::make_shared<Clip>();
				clip->baseAudioSource = clipPathString;

				// TODO: rewrite the path stuff in this function
				auto path = fs::path(clipPathString);
				clip->name = path.stem().string();

				// ma_uint64 len = 0;
				// res = ma_sound_init_from_file(audioEngine, clipPath, soundFlags, nullptr, nullptr, &clip->engineSound);
				// if (res != MA_SUCCESS) EC_ERROUT(EC_THIS, "Engine failure to load audio from file! Err enum %i", res);
				// else clip->loaded = true;
				// res = ma_data_source_get_length_in_pcm_frames(&clip->engineSound, &len);
				// if (res != MA_SUCCESS) EC_ERROUT(EC_THIS, "Engine failure calc clip length! Err enum %i", res);

				ma_decoder decoder;
				res = ma_sound_init_from_file(audioEngine, clipPath, soundFlags, &songInfo->tracks.at(tableHoveredRow - 1).soundGroup, nullptr, &clip->engineSound);
				checkResult(res);
				res = ma_decoder_init_file(clipPath, nullptr, &decoder);
				checkResult(res);

				ma_uint64 len;
				res = ma_decoder_get_length_in_pcm_frames(&decoder, &len);
				checkResult(res);
				len = len / 100;

				clip->loaded = true;

				ma_decoder_uninit(&decoder);


				// see the scrubber position set code if you want to comprehend this math
				float relativePixelOffset = GetMousePos().x - GetCursorScreenPos().x;
				float pcmFramesToBeSet = (relativePixelOffset * 100) / (editorState->zoom / 100.0f);

				// clip->length = (uint64_t)len;
				clip->length = len;
				clip->position = static_cast<uint64_t>(std::max(0.0f, pcmFramesToBeSet / 100.0f));

				EC_PRINT(EC_THIS, "user dropped %s", clipPath);

				songInfo->tracks.at(tableHoveredRow - 1).clips.push_back(clip);

				// Load into JUCE File
				juce::File audioData(path.string());
				juce::FileInputSource inputSource(audioData);
				clip->thumbnail.setSource(&inputSource);
			}
		} else {
			fg->AddCircleFilled(GetMousePos(), 5.0f, IM_COL32(255, 0, 0, 255));
			fg->AddText(ImVec2(mouse.x, mouse.y - 40.0f), IM_COL32(255, 0, 0, 255), "Not a track!");
		}
		EndDragDropTarget();
	}

	onUpdateBookmarks(drawList);

	End();
}

static NodeGraph* getMasterNodeGraph(Song* song) {
	for (auto& g : song->nodeGraphs) {
		if (g.name == "internal_master_node_graph") return &g;
	}
	return nullptr;
}

void Timeline::updateTrackAutomationsPopup() {
	using namespace ImGui;

	if (BeginPopup("Track Automations")) {

		if (currentlySelectedTrack == nullptr) {
			SeparatorText("No track selected!");
		} else {
			SeparatorText(("Autos for " + currentlySelectedTrack->name).c_str());

			int autoIt = 1;
			for (auto& automation : currentlySelectedTrack->automations) {
				PushID(autoIt);
				ToggleButton(SHADOW_ICON_EYE, &automation.visible);
				SetItemTooltip("Toggle Automation Visibility");
				SameLine();
				ColorEdit4("##AutoColor", (float*)&automation.color);
				SameLine();
				ToggleButton(automation.smoothCurve ? SHADOW_ICON_CURVED_PATH : SHADOW_ICON_STRAIGHT_PATH, &automation.smoothCurve);
				SameLine();
				if (Button(SHADOW_ICON_CLOSE))
					currentlySelectedTrack->automations.erase(currentlySelectedTrack->automations.begin() + autoIt - 1);
				SetItemTooltip("Delete Automation");
				PopID();
				autoIt++;
			}

			if (Selectable("+ Add Automation...", false, ImGuiSelectableFlags_DontClosePopups)) {
				Automation newAuto;
				newAuto.points.push_back(std::pair<uint64_t, float>(20, 10.0f));
				currentlySelectedTrack->automations.push_back(newAuto);
				auto master = getMasterNodeGraph(songInfo);
				if (master == nullptr) {
					JobSystem::degradeEditorWithMessage(
						"Automation creation failure",
						"Can't find the master node graph, this shouldn't happpen?");
				} else {
					// What am I even doing here?
					Node n;
					n.name = "Generated auto track data : " + currentlySelectedTrack->name;
					n.color = ImColor(255, 0, 0, 100);
					n.type = NodeType_Regular;
					n.id = songInfo->lastKnownGraphId++;

					Pin output;
					output.id = songInfo->lastKnownGraphId++;
					output.name = "Audio Out";

					n.outputs.emplace_back(std::move(output));

					master->nodes.emplace_back(std::move(n));

					Link l;
					l.id = songInfo->lastKnownGraphId++;
					l.inputId = output.id.Get();

					for (auto& iteratedNode : master->nodes) {
						if (iteratedNode.type == NodeType_Output) {
							l.outputId = iteratedNode.inputs.at(0).id.Get(); // technically hacky, whatever.
							master->links.push_back(l);
							break;
						}
					}
				}
			}
		}

		EndPopup();
	}
}

void Timeline::startPlayback() {
	playing = true;

	EC_WARN(EC_THIS, "Clips To Play");
	for (auto& track : songInfo->tracks) {
		// EC_PRINT(EC_THIS, "-- TRACK %s", track.name.c_str());
		for (auto& clip : track.clips) {
			if (clip->position + clip->length < playbackFrames) continue;

			if (clip->position > playbackFrames) {
				EC_PRINT(EC_THIS, "CLIP %s is in the future\nWill play at %lu", clip->name.c_str(), clip->position);

				if (clip->clipType == TimelineClipType_Audio) {
					PPK_ASSERT(clip->loaded, "Sound data not loaded for this clip!");
	
					ma_sound_seek_to_pcm_frame(&clip->engineSound, 0);
					ma_sound_set_start_time_in_pcm_frames(&clip->engineSound, clip->position * 100);
					ma_sound_start(&clip->engineSound);
				} else if (clip->clipType == TimelineClipType_Drums) {
					// Clip doesn't have to be loaded as it doesn't necessarily
					// have a sound associated with it. It's more like a collection!
					EC_PRINT(EC_THIS, "Playing drum collection in the future!");
					drumEngine->scheduleDrumPlayback(clip.get(), clip->position * 100);
				}
			} else {
				EC_PRINT(EC_THIS, "CLIP %s is on the line", clip->name.c_str());
			}
		}
	}

	ma_engine_start(audioEngine);
}

void Timeline::stopPlayback() {
	playing = false;

	ma_engine_stop(audioEngine);
}

void Timeline::togglePlayback() {
	playing = !playing;

	if (playing)
		startPlayback();
	else
		stopPlayback();
}

void Timeline::startClipDragging() {
	mouseXonDragStart = ImGui::GetMousePos().x;

	// for (auto& selectedClip : selectedClips) {
	// 	selectedClip.
	// }
}

void Timeline::addTrack(const std::string& name) {
	Track newTrack;
	newTrack.name = name;
	songInfo->tracks.emplace_back(newTrack);

	// Hacky

	auto& initedTrack = songInfo->tracks.back();

	ma_result res;
	res = ma_sound_group_init(audioEngine, MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, &initedTrack.soundGroup);
	checkResult(res);

	// This stuff is all default, idrk if I have to do this, but it doesn't hurt.
	ma_sound_group_set_volume(&initedTrack.soundGroup, (initedTrack.volume / 100.0f));
	ma_sound_group_set_pan_mode(&initedTrack.soundGroup, initedTrack.realPanning ? ma_pan_mode_pan : ma_pan_mode_balance);
	ma_sound_group_set_pan(&initedTrack.soundGroup, initedTrack.balance);
}

void Timeline::newDrumCollection(Track* track, uint64_t position) {

	auto clip = std::make_shared<Clip>();

	clip->name = "Untitled Drum Collection";
	clip->position = position;
	clip->length = 400;
	clip->clipType = TimelineClipType_Drums;
	clip->drumData = std::make_shared<DrumMachineData>();

	ma_result res;
	res = ma_sound_group_init(audioEngine, MA_SOUND_FLAG_NO_SPATIALIZATION, &track->soundGroup, &clip->drumData->drumSoundGroup);
	checkResult(res);

	track->clips.emplace_back(clip);
}

void Timeline::newPianoRoll(Track* track, uint64_t position) {

	auto clip = std::make_shared<Clip>();

	clip->name = "Untitled Piano Roll";
	clip->position = position;
	clip->length = 400;
	clip->clipType = TimelineClipType_PianoRoll;
	clip->pianoRollData = std::make_shared<PianoRollData>();

	track->clips.emplace_back(clip);
}

}
