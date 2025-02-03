#define IMGUI_DEFINE_MATH_OPERATORS
#include "Configuration/EngineConfiguration.hpp"
#include "Debug/Logger.hpp"
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
#include "IconsCodicons.h"
#include "imgui/imgui_utils.hpp"
#include "Debug/EditorConsole.hpp"
#include "ppk_assert_impl.hpp"
#include <memory>
#include "ImGuiNotify.hpp"
#include "json_impl.hpp"
#include "imgui/imgui_api_patches.hpp"

#define EC_THIS "Timeline"

namespace Shadow::AXE {

// static float oset = 0.0f;

Timeline::Timeline(Song* songInfo, EditorState* editorState, ma_engine* audioEngine, ShadowWindow* window)
	: songInfo(songInfo)
	, editorState(editorState)
	, audioEngine(audioEngine)
	, window(window)
		{
			EC_NEWCAT(EC_THIS);
			EC_PRINT(EC_THIS, "Scale factor from editorstate %.2f", editorState->sf);
		};
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
		if (SmallButton(ICON_CI_CLOSE)) songInfo->tracks.erase(songInfo->tracks.begin() + trackIt);
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
			if (Button(ICON_CI_CLOSE)) track.clips.erase(track.clips.begin() + clipIt - 1);

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

void Timeline::onUpdate() {
	using namespace ImGui;

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

	if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) &&
		IsKeyPressed(GetKeyIndex(ImGuiKey_Space)) &&
		!GetIO().WantTextInput)
		togglePlayback();

	// ImDrawList uses screen coords NOT window coords
	ImDrawList* drawList = GetWindowDrawList();
	ImVec2 canvasPos = GetCursorScreenPos();
	ImVec2 canvasSize = GetContentRegionAvail();
	ImVec2 canvasMax = GetWindowContentRegionMax();

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

		int trackIt = 0;
		for (auto& track : songInfo->tracks) {
			PushID(trackIt);
			TableNextRow();
			TableSetColumnIndex(0);

			PushItemWidth(200.0f * sf);
			InputText("##TrackName", &track.name);
			SameLine();
			if (SmallButton(ICON_CI_CLOSE)) songInfo->tracks.erase(songInfo->tracks.begin() + trackIt);
			SliderFloat("VOL", &track.volume, 0.0f, 100.0f, "%.0f");
			SliderFloat("BAL", &track.balence, -1.0f, 1.0f, "%.2f");
			ToggleButton("M", &track.muted);
			SetItemTooltip("Mute Track");
			// ToggleButton("S", )

			SameLine();
			if (Button(ICON_CI_ROBOT)) {
				currentlySelectedTrack = &track;
				OpenPopup("Track Automations");
			}
			SetItemTooltip("Track Automations");
			updateTrackAutomationsPopup();

			TableSetColumnIndex(1);

			PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0,0,0,0));

			// top left of the cell
			ImVec2 screenPosOrigin = GetCursorScreenPos();

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
					if (SmallButton(ICON_CI_CLOSE)) track.clips.erase(track.clips.begin() + clipIt + 1);
				}
				// if (SmallButton(ICON_CI_CHROME_RESTORE)) {
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
			InvisibleButton("##SameLineStopper", ImVec2(1,1));

			PopStyleColor();

			int autoit = 1;
			for (auto& automation : track.automations) {

				if (!automation.visible) continue;

				PushID(autoit);

				ImColor col = automation.color;
				ImRect bounds = ImRect(screenPosOrigin.x, screenPosOrigin.y, GetWindowPos().x + GetWindowWidth(), screenPosOrigin.y + 143.0f);

				ImDrawList* fg = GetForegroundDrawList();

				if (editorState->automationDebugMode) {
					fg->AddRect(bounds.Min, bounds.Max, col, 2.0f, 0, 2);
					fg->AddText(bounds.Min + ImVec2(10,10), col, "AUTOMATION DEBUG");
				}

				PushClipRect(bounds.Min, bounds.Max, false);

				{ // Start Rail
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
				}

				size_t pointit = 1;
				for (auto& point : automation.points) {
					PushID(pointit);
					uint64_t x = point.first;
					float y = point.second;

					SetCursorScreenPos(ImVec2(bounds.Min.x + x - 10, bounds.Min.y + y - 10));
					InvisibleButton("PointGrabber", ImVec2(20,20));
					if (IsItemHovered() && IsMouseDown(0) && !autoPointBeingDragged) {
						autoPointBeingDragged = GetItemID();
					} else if (IsMouseReleased(0)) autoPointBeingDragged = 0;
					if (autoPointBeingDragged == GetItemID()) {
						// Actively dragging
						ImVec2 mousePos = GetMousePos(), winPos = GetWindowPos();
						point.first = mousePos.x - winPos.x - 376.0f;
						point.second = std::max(0.0f, std::min(130.0f, mousePos.y - screenPosOrigin.y));
					}

					drawList->AddCircleFilled(
						ImVec2(bounds.Min.x + x, bounds.Min.y + y),
						10.0f,
						col,
						4
					);

					drawList->AddCircleFilled(
						ImVec2(bounds.Min.x + x, bounds.Min.y + y),
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
						const ImVec2 p1 = ImVec2(bounds.Min.x + point.first, bounds.Min.y + point.second);
						const ImVec2 p2 = ImVec2(bounds.Min.x + nextPoint.first, bounds.Min.y + nextPoint.second);

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
		if (Button(ICON_CI_PLUS " Add Track")) {
			Track newTrack;
			newTrack.name = "Untitled Track " + std::to_string(trackIt + 1);
			songInfo->tracks.push_back(newTrack);
		}
#endif

		// GetForegroundDrawList()->AddText(ImVec2(900, 700), IM_COL32(255, 255, 255, 255), std::to_string(TableGetHoveredRow()).c_str());
		tableHoveredRow = TableGetHoveredRow();

		if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Middle)) {
			if (GetIO().KeyCtrl) {
				editorState->zoom -= scrollDelta.x;
				if (editorState->zoom < 10) editorState->zoom = 10;
			} else {
				SetScrollX(GetScrollX() + scrollDelta.x);
				SetScrollY(GetScrollY() + scrollDelta.y);
			}
		}

		if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Right)) {
			ma_engine_set_time_in_pcm_frames(audioEngine, uint64_t(std::max(0.0f, GetMousePos().x - GetWindowPos().x + GetScrollX() - 376.0f) * editorState->zoom));
		}

		PushClipRect(
			ImVec2(canvasPos.x + (245*sf), canvasPos.y),
			ImVec2(canvasPos.x + canvasMax.x, canvasPos.y + canvasMax.y + 1000.0f),
			false
		);

		ImDrawList* tableDrawList = GetWindowDrawList();

		// Draw Scrubber
		float scrubberWindowPos = ((250 * sf) + playbackFrames - GetScrollX()); // TODO: this broke w/ zoom addon
		tableDrawList->AddTriangleFilled(
			ImVec2(canvasPos.x + scrubberWindowPos - 10.0f, canvasPos.y + 0.0f),
			ImVec2(canvasPos.x + scrubberWindowPos + 10.0f, canvasPos.y + 0.0f),
			ImVec2(canvasPos.x + scrubberWindowPos + 1.0f, canvasPos.y + 33.0f),
			IM_COL32(255, 0, 0, 255)
		);
		tableDrawList->AddRectFilled(
			ImVec2(canvasPos.x + scrubberWindowPos, canvasPos.y + 10.0f),
			ImVec2(canvasPos.x + scrubberWindowPos + 2.0f, canvasMax.y + 100.0f),
			IM_COL32(255, 0, 0, 255)
		);

		// Draw Scale / Ruler
		ImColor textColor = GetStyle().Colors[ImGuiCol_Text];

		float majorUnit =		100.0f * (editorState->zoom / 100.0f);
		float minorUnit =		10.0f * (editorState->zoom / 100.0f);
		float labelAlignment =	0.6f;
		float sign =			1.0f;
		float minorSize =		10.0f;
		float majorSize =		30.0f;
		float labelDistance =	8.0f;

		ImVec2 from = ImVec2(canvasPos.x + (245*sf) - GetScrollX(), canvasPos.y);
		ImVec2 to = ImVec2(canvasPos.x + canvasMax.x, canvasPos.y);

		ImVec2 direction = (to - from) * ImInvLength(to - from, 0.0f);
		ImVec2 normal    = ImVec2(direction.y, direction.x);
		float distance   = sqrtf(ImLengthSqr(to - from));

		tableDrawList->AddLine(from, to, textColor);

		auto p = from;
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

				if (editorState->zoom < 50.0f) continue;

				char label[16];
				snprintf(label, 15, "%g", d * sign);
				ImVec2 labelSize = ImGui::CalcTextSize(label);

				ImVec2 labelPosition = p + ImVec2(fabsf(normal.x), fabsf(normal.y)) * labelDistance;
				float labelAlignedSize = ImDot(labelSize, direction);
				labelPosition += direction * (-labelAlignedSize + labelAlignment * labelAlignedSize * 2.0f);
				labelPosition = ImFloor(labelPosition + ImVec2(0.5f, 0.5f));

				tableDrawList->AddText(labelPosition, textColor, label);
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
				clip->name = "Untitled Clip";

				// ma_uint64 len = 0;
				// res = ma_sound_init_from_file(audioEngine, clipPath, soundFlags, nullptr, nullptr, &clip->engineSound);
				// if (res != MA_SUCCESS) EC_ERROUT(EC_THIS, "Engine failure to load audio from file! Err enum %i", res);
				// else clip->loaded = true;
				// res = ma_data_source_get_length_in_pcm_frames(&clip->engineSound, &len);
				// if (res != MA_SUCCESS) EC_ERROUT(EC_THIS, "Engine failure calc clip length! Err enum %i", res);

				ma_decoder decoder;
				res = ma_sound_init_from_file(audioEngine, clipPath, soundFlags, nullptr, nullptr, &clip->engineSound);
				res = ma_decoder_init_file(clipPath, nullptr, &decoder);

				ma_uint64 len;
				res = ma_decoder_get_length_in_pcm_frames(&decoder, &len);
				len = len / 100;

				clip->loaded = true;

				ma_decoder_uninit(&decoder);


				// clip->length = (uint64_t)len;
				clip->length = len;
				clip->position = static_cast<uint64_t>(GetMousePos().x - GetWindowPos().x);

				EC_PRINT(EC_THIS, "user dropped %s", clipPath);

				songInfo->tracks.at(tableHoveredRow - 1).clips.push_back(clip);
			}
		} else {
			fg->AddCircleFilled(GetMousePos(), 5.0f, IM_COL32(255, 0, 0, 255));
			fg->AddText(ImVec2(mouse.x, mouse.y - 40.0f), IM_COL32(255, 0, 0, 255), "Not a track!");
		}
		EndDragDropTarget();
	}

	End();
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
				ToggleButton(ICON_CI_EYE, &automation.visible);
				SetItemTooltip("Toggle Automation Visibility");
				SameLine();
				ColorEdit4("##AutoColor", (float*)&automation.color);
				SameLine();
				if (Button(ICON_CI_CLOSE))
					currentlySelectedTrack->automations.erase(currentlySelectedTrack->automations.begin() + autoIt - 1);
				SetItemTooltip("Delete Automation");
				PopID();
				autoIt++;
			}

			if (Selectable("+ Add Automation...", false, ImGuiSelectableFlags_DontClosePopups)) {
				Automation newAuto;
				newAuto.points.push_back(std::pair<uint64_t, float>(20, 10.0f));
				currentlySelectedTrack->automations.push_back(newAuto);
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

				PPK_ASSERT(clip->loaded, "Sound data not loaded for this clip!");

				ma_sound_seek_to_pcm_frame(&clip->engineSound, 0);
				ma_sound_set_start_time_in_pcm_frames(&clip->engineSound, clip->position * 100);
				ma_sound_start(&clip->engineSound);
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
}
