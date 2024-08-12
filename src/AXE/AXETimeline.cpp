#define IMGUI_DEFINE_MATH_OPERATORS
#include "AXETimeline.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <string>
#include "IconsCodicons.h"
#include "imgui/imgui_utils.hpp"
#include "Debug/EditorConsole.hpp"
#include "ppk_assert_impl.hpp"

#define EC_THIS "Timeline"

namespace Shadow::AXE {

// static float oset = 0.0f;

Timeline::Timeline(Song* songInfo, EditorState* editorState, ma_engine* audioEngine)
	: songInfo(songInfo)
	, editorState(editorState)
	, audioEngine(audioEngine)
		{
			EC_NEWCAT(EC_THIS);
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

	ImGuiTableFlags tblFlags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
	if (BeginTable("TimelineTable", 2, tblFlags)) {

		TableSetupScrollFreeze(1, 1);
		TableSetupColumn("Track Properties");
		TableSetupColumn("Timeline", ImGuiTableColumnFlags_NoHeaderLabel);
		TableHeadersRow();

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
			Checkbox("Muted", &track.muted);

			TableSetColumnIndex(1);

			PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0,0,0,0));

			int clipIt = 0;
			float left = GetCursorPosX();
			for (auto& clip : track.clips) {
				PushID(clipIt);
				SetCursorPosX(left + (float)clip.position * (editorState->zoom / 100.0f));
				float clipWidth = (float)clip.length * (editorState->zoom / 100.0f);
				BeginChild("Clip", ImVec2(clipWidth, 143.0f), 0, 0);

				// Clip dragging logic
				if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Left) && !clipBeingDragged) {
					clipBeingDragged = &clip;
					clipStoredMouseOffsetX = GetMousePos().x - GetWindowPos().x;
				} else if (IsMouseReleased(ImGuiMouseButton_Left)) clipBeingDragged = nullptr;

				if (clipBeingDragged == &clip) {
					// Actively dragging
					clip.position = std::max(0.0f, clip.position + GetMousePos().x - GetWindowPos().x - clipStoredMouseOffsetX);
					#if 0 //TODO: this is bugged :P
					if (!IsWindowHovered()) {
						int n = GetMouseDragDelta(0).y < 0.0f ? -1 : 1;
						EC_PRINT(EC_THIS, "Clip swap %i", n);
						songInfo->tracks.at(trackIt + n).clips.push_back(clip);
						track.clips.erase(track.clips.begin() + clipIt + 1);
					}
					#endif
				}

				SetNextItemWidth(clipWidth - 55.0f);
				InputText("##ClipName", &clip.name);
				SameLine();
				if (SmallButton(ICON_CI_CLOSE)) track.clips.erase(track.clips.begin() + clipIt + 1);
				Text("pos (u64) %lu\nlength (u64) %lu\npos + len (end rail pos) %lu",
					clip.position,
					clip.length,
					clip.position + clip.length);

				EndChild();
				PopID();
				SameLine();
				clipIt++;
			}
			InvisibleButton("##SameLineStopper", ImVec2(1,1));

			PopStyleColor();

			PopID();
			trackIt++;
		}

		TableNextRow();
		TableSetColumnIndex(0);
		if (Button(ICON_CI_PLUS " Add Track")) {
			Track newTrack;
			newTrack.name = "Untitled Track " + std::to_string(trackIt + 1);
			songInfo->tracks.push_back(newTrack);
		}

		if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Middle)) {
			if (GetIO().KeyCtrl) {
				editorState->zoom -= scrollDelta.x;
				if (editorState->zoom < 5) editorState->zoom = 5;
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

		tableDrawList->AddLine(from, to, IM_COL32(255, 255, 255, 255));

		auto p = from;
		if (editorState->zoom > 70.0f)
			for (float d = 0.0f; d <= distance; d += minorUnit, p += direction * minorUnit)
				tableDrawList->AddLine(p - normal * minorSize, p + normal * minorSize, IM_COL32(255, 255, 255, 255));

		if (editorState->zoom > 20.0f)
			for (float d = 0.0f; d <= distance + majorUnit; d += majorUnit) {
				p = from + direction * d;

				tableDrawList->AddLine(p - normal * majorSize, p + normal * majorSize, IM_COL32(255, 255, 255, 255));
				tableDrawList->AddLine(p - normal * canvasMax.y, p + normal * canvasMax.y, IM_COL32(255, 255, 255, 50));

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

				tableDrawList->AddText(labelPosition, IM_COL32(255, 255, 255, 255), label);
			}

		PopClipRect();

		EndTable();
	}

	End();
	PopStyleVar(2);
}

void Timeline::startPlayback() {
	playing = true;

	EC_WARN(EC_THIS, "Clips To Play");
	for (auto& track : songInfo->tracks) {
		// EC_PRINT(EC_THIS, "-- TRACK %s", track.name.c_str());
		for (auto& clip : track.clips) {
			if (clip.position + clip.length < playbackFrames) continue;
			
			if (clip.position > playbackFrames) {
				EC_PRINT(EC_THIS, "CLIP %s is in the future\nWill play at %lu", clip.name.c_str(), clip.position);

				PPK_ASSERT(clip.loaded, "Sound data not loaded for this clip!");
				
				ma_sound_seek_to_pcm_frame(&clip.engineSound, 0);
				ma_sound_set_start_time_in_pcm_frames(&clip.engineSound, clip.position * 100);
				ma_sound_start(&clip.engineSound);
			} else {
				EC_PRINT(EC_THIS, "CLIP %s is on the line", clip.name.c_str());
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