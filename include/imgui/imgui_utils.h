#ifndef IMGUI_UTILS_H
#define IMGUI_UTILS_H

#include <imgui.h>
#include <string>

namespace ImGui {
inline bool MouseOverArea() {
	return false || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered()
		|| ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}

bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0,
	ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
}

#endif /* IMGUI_UTILS_H */