#include "AXEPianoRoll.hpp"
#include "imgui.h"
#include "IconsCodicons.h"

namespace Shadow::AXE {

PianoRoll::PianoRoll() { }

PianoRoll::~PianoRoll() { }

static bool hasBlack(int key) {
	// I know so little about music holy cannoli
	return (!((key - 1) % 7 == 0 || (key - 1) % 7 == 3) && key != 51);
}

static int key_states[256] = {0};

void PianoRoll::onUpdate(bool& p_open) {
	using namespace ImGui;

	if (!p_open) return;

	if (!Begin("Piano Roll", &p_open)) {
		End();
		return;
	}

	TextUnformatted(ICON_CI_FILE " Working on: ");

	ImU32 Black = IM_COL32(0, 0, 0, 255);
	ImU32 White = IM_COL32(255, 255, 255, 255);
	ImU32 Red = IM_COL32(255,0,0,255);

	ImDrawList *draw_list = GetWindowDrawList();
	ImVec2 p = GetCursorScreenPos();
	int width = 20;
	int cur_key = 21;
	for (int key = 0; key < 52; key++) {
		ImU32 col = White;
		if (key_states[cur_key]) {
			col = Red;
		}
		draw_list->AddRectFilled(
				ImVec2(p.x + key * width, p.y),
				ImVec2(p.x + key * width + width, p.y + 120),
				col, 0, ImDrawFlags_RoundCornersAll);
		draw_list->AddRect(
				ImVec2(p.x + key * width, p.y),
				ImVec2(p.x + key * width + width, p.y + 120),
				Black, 0, ImDrawFlags_RoundCornersAll);
		cur_key++;
		if (hasBlack(key)) {
			cur_key++;
		}
	}
	cur_key = 22;
	for (int key = 0; key < 52; key++) {
		if (hasBlack(key)) {
			ImU32 col = Black;
			if (key_states[cur_key]) {
				col = Red;
			}
			draw_list->AddRectFilled(
					ImVec2(p.x + key * width + width * 3 / 4, p.y),
					ImVec2(p.x + key * width + width * 5 / 4 + 1, p.y + 80),
					col, 0, ImDrawFlags_RoundCornersAll);
			draw_list->AddRect(
					ImVec2(p.x + key * width + width * 3 / 4, p.y),
					ImVec2(p.x + key * width + width * 5 / 4 + 1, p.y + 80),
					Black, 0, ImDrawFlags_RoundCornersAll);

			cur_key += 2;
		} else {
			cur_key++;
		}
	}

	End();

}

}
