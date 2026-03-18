#define IMGUI_DEFINE_MATH_OPERATORS
#include "AXETimeline.hpp"
#include "AXETypes.hpp"
#include "ShadowIcons.hpp"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include <string>
#include <utility>

namespace Shadow::AXE {

void Timeline::newBookmark() {
	Bookmark bm;

	bookmarkInc++;

	bm.name = "Bookmark " + std::to_string(bookmarkInc);
	bm.color = ImColor(255, 0, 0);
	bm.position = 0;

	songInfo->bookmarks.emplace_back(std::move(bm));
}

// Function responsible for displaying the user made bookmarks on the Timeline
void Timeline::onUpdateBookmarks(ImDrawList* drawDest) {
	using namespace ImGui;

	auto winPos = GetWindowPos();
	auto winSize = GetWindowSize();

	drawDest->AddRectFilled(ImVec2(winPos.x, winPos.y), winPos + winSize, IM_COL32(255, 0, 0, 10));

	auto currentCursorPos = GetCursorScreenPos();

	for (auto& bookmark : songInfo->bookmarks) {
		
		auto bookmarkTimelinePosition = static_cast<float>(bookmark.position) * (editorState->zoom / 100.0f);

		// Actual bookmark drawing.
		// Negative Y values allows it to be shown in the gutter on the top instead of in the
		// timeline itself. This drawing is broken up into two parts. The first is the rectangle,
		// a fat part that the user can grab easily with their mouse but also easily see visually.
		// And the second part, a triangle on the bottom that brings the rectangle in and displays
		// the exact point where the bookmark lives on the timeline.
		drawDest->AddRectFilled( // ...The rectangle
			currentCursorPos + ImVec2(bookmarkTimelinePosition - 10.f, -20.0f),
			currentCursorPos + ImVec2(bookmarkTimelinePosition + 10.0f, 0.0f),
			bookmark.color
		);
		// ...Triangle on the bottom
		drawDest->PathLineTo(currentCursorPos + ImVec2(bookmarkTimelinePosition - 10.0f, 0.0f)); // Top-left
		drawDest->PathLineTo(currentCursorPos + ImVec2(bookmarkTimelinePosition + 10.0f, 0.0f)); // Top-right
		drawDest->PathLineTo(currentCursorPos + ImVec2(bookmarkTimelinePosition + 0.0f, 14.0f)); // Bottom-center
		drawDest->PathFillConvex(bookmark.color);

		// Mouse hit testing...
		// Here we detect when the user is hovering the bookmark and draw its name and position
		// nearby in the draw-list

		if (IsMouseHoveringRect(currentCursorPos + ImVec2(bookmarkTimelinePosition - 10.f, -20.0f), currentCursorPos + ImVec2(bookmarkTimelinePosition + 10.0f, -5.0f))) {
			SetTooltip("%s", std::string(bookmark.name + "\nAT... " + std::to_string(bookmark.position)).c_str());
			// drawDest->AddText(currentCursorPos + ImVec2(bookmarkTimelinePosition + 15.0f, 3.0f), IM_COL32(255, 255, 255, 255), bookmark.name.c_str());
			SetMouseCursor(ImGuiMouseCursor_Hand);
			// if (IsMouseDown(ImGuiMouseButton_Left)) {
			// 	// User is grabbing
			// 	window->setSECursor(ShadowEngineCursors_Grab);
			// }
		}
	}
}

// A window that visually shows the user all the bookmarks that they have placed in the song. From
// here they can click to jump to them, see basic info about them, delete, and edit them.
void Timeline::updateBookmarkWindow(bool& p_open) {
	using namespace ImGui;

	if (!p_open) return;

	if (!Begin("Your Bookmarks", &p_open)) {
		End();
		return;
	}

	SeparatorText("Bookmarks");

	int it = 0;
	for (auto& bookmark : songInfo->bookmarks) {
		// TODO: we probably also want to show the bookmark's position or have a jump button here too
		PushID(it);

		ColorEdit4("##BookmarkColor", reinterpret_cast<float*>(&bookmark.color), ImGuiColorEditFlags_NoInputs);
		SameLine();
		InputText("##BookmarkName", &bookmark.name);
		SameLine();
		SetNextItemWidth(50.0f);
		InputScalar("##BookmarkPosition", ImGuiDataType_U64, &bookmark.position);
		SameLine();
		if (SmallButton(SHADOW_ICON_CLOSE))
			songInfo->bookmarks.erase(songInfo->bookmarks.begin() + it);

		PopID();
		it++;
	}

	End();
}

}