#include "AXETimeline.hpp"
#include "Debug/Logger.hpp"
#include "imgui.h"
#include <string>
#include "IconsCodicons.h"
#include "imgui/imgui_utils.hpp"

namespace Shadow::AXE {

Timeline::Timeline(Song* songInfo, EditorState* editorState, ma_engine* audioEngine)
	: songInfo(songInfo)
	, editorState(editorState)
	, audioEngine(audioEngine)
		{ };
Timeline::~Timeline() { }

void Timeline::onUpdate() {
	using namespace ImGui;

	float sf = editorState->sf;

	Begin("Timeline");

	// ImDrawList uses screen coords NOT window coords
	ImDrawList* drawList = GetWindowDrawList();
	ImVec2 canvasPos = GetCursorScreenPos();
	// ImVec2 canvasSize = GetContentRegionAvail();

	// For middle click drag
	ImVec2 mouseDelta = GetIO().MouseDelta;
	ImVec2 scrollDelta = ImVec2(-mouseDelta.x, -mouseDelta.y);

	if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Middle)) {
		SetScrollY(GetScrollY() + scrollDelta.y);
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
			float clipWidth = 300.0f;
			SetCursorPos(ImVec2(clip.position, 2.0f));
			BeginChild("clip", ImVec2(clipWidth, GetWindowSize().y - 4.0f), 0, 0);

			if (IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IsMouseDown(ImGuiMouseButton_Left)) {
				clip.position = std::max(0.0f, clip.position + GetMousePos().x - GetWindowPos().x - (clipWidth / 2));
			}

			SetNextItemWidth(clipWidth - 40.0f);
			InputText("##name", &clip.name);
			SameLine();
			if (Button(ICON_CI_CLOSE)) track.clips.erase(track.clips.begin() + clipIt - 1);

			Text("Window x: %.3f\nmouse x: %.3f\noffset: %.3f",
				GetWindowPos().x,
				GetMousePos().x,
			GetMousePos().x - GetWindowPos().x);

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
				// clip.position = static_cast<uint64_t>(GetMousePos().x - GetWindowPos().x);
				clip.position = GetMousePos().x - GetWindowPos().x;

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
	DragScalar("PCM Frames", ImGuiDataType_U64, &playbackFrames);
	float scrubberWindowPos = (260 * sf) + playbackFrames;
	fg->AddTriangleFilled(
		ImVec2(canvasPos.x + scrubberWindowPos - 10.0f, canvasPos.y + 30.0f),
		ImVec2(canvasPos.x + scrubberWindowPos + 10.0f, canvasPos.y + 30.0f),
		ImVec2(canvasPos.x + scrubberWindowPos, canvasPos.y + 60.0f),
		IM_COL32(255, 0, 0, 255)
	);
	fg->AddRectFilled(ImVec2(canvasPos.x + scrubberWindowPos, canvasPos.y + 60.0f), ImVec2(canvasPos.x + scrubberWindowPos + 2.0f, 10000.0f), IM_COL32(255, 0, 0, 255));

	End();
}

void Timeline::startPlayback() {
	for (auto& track : songInfo->tracks) {
		for (auto& clip : track.clips) {
			// if (clip.position > current engine pos)
			//     get ma_sound and set to play
		}
	}
}


}