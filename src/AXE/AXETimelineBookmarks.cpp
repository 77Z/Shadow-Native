#define IMGUI_DEFINE_MATH_OPERATORS
#include "AXETimeline.hpp"
#include "AXETypes.hpp"
#include "imgui.h"
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

void Timeline::onUpdateBookmarks(ImDrawList* drawDest) {
	using namespace ImGui;

	auto winPos = GetWindowPos();
	auto winSize = GetWindowSize();

	// drawDest->AddRectFilled(ImVec2(winPos.x + 385, winPos.y), winPos + winSize, IM_COL32(255, 0, 0, 10));

	for (auto& bookmark : songInfo->bookmarks) {
		
		auto basePos = ImVec2(385 + (float)bookmark.position + winPos.x, winPos.y + 100);
		drawDest->AddRectFilled(basePos, basePos + ImVec2(50, 50), bookmark.color);
		drawDest->AddRectFilled(basePos, ImVec2(basePos.x, basePos.y + winSize.y), bookmark.color);
		drawDest->AddText(basePos, IM_COL32(255, 255, 255, 255), bookmark.name.c_str());
	}
}

void Timeline::updateBookmarkDebugMenu(bool& p_open) {
	using namespace ImGui;

	if (!p_open) return;

	Begin("Timeline Bookmark Debugger", &p_open);

	TextUnformatted("Bookmarks in this song file:");

	for (auto& g : songInfo->bookmarks) {
		ColorButton("BOOKMARK", g.color);
		SameLine();
		TextUnformatted((g.name + " @ " + std::to_string(g.position)).c_str());
	}

	End();
}

}