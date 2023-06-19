#ifndef IMGUI_UTILS_H
#define IMGUI_UTILS_H

#include <imgui.h>

namespace ImGui {
	inline bool MouseOverArea() {
		return false
			|| ImGui::IsAnyItemActive()
			|| ImGui::IsAnyItemHovered()
			|| ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
	}
}

#endif /* IMGUI_UTILS_H */