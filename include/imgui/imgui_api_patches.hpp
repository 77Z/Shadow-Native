#ifndef SHADOW_NATIVE_IMGUI_IMGUI_API_PATCHES_HPP
#define SHADOW_NATIVE_IMGUI_IMGUI_API_PATCHES_HPP

#include "imgui.h"

// Null out this function due to ImGui API Update :P
namespace ImGui {
// inline ImGuiKey GetKeyIndex(ImGuiKey k) { return k; }
}

#endif /* SHADOW_NATIVE_IMGUI_IMGUI_API_PATCHES_HPP */