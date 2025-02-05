#ifndef SHADOW_NATIVE_IMGUI_THEME_HPP
#define SHADOW_NATIVE_IMGUI_THEME_HPP

#include "imgui.h"

namespace ImGui {

inline void SetupTheme() {
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.WindowRounding = 7.0f;
	style.FrameRounding = 2.0f;

	style.WindowTitleAlign.x = 0.5f;

	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	// style.Colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.00f, 0.50f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.10f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.54f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.20f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.32f, 0.32f, 0.32f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.04f, 0.04f, 0.04f, 0.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(1.00f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.43f, 0.43f, 1.00f);
	// style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.53f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(1.00f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.43f, 0.43f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 0.17f, 0.17f, 0.31f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 0.17f, 0.17f, 0.54f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 0.17f, 0.17f, 0.31f);
	style.Colors[ImGuiCol_Tab] = ImVec4(1.00f, 0.17f, 0.17f, 0.50f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(1.00f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(1.00f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.14f, 0.97f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(1.00f, 0.17f, 0.17f, 0.31f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.17f, 0.17f, 0.39f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.13f, 0.13f, 0.13f, 0.00f);
	style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.98f, 0.26f, 0.26f, 0.70f);
	style.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
}

inline void SetupSadTheme() {
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.10f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border]                 = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.10f, 0.10f, 0.10f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.20f, 0.20f, 0.20f, 0.40f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.32f, 0.32f, 0.32f, 0.67f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.04f, 0.04f, 0.04f, 0.00f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
	colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(1.00f, 1.00f, 1.00f, 0.54f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(1.00f, 1.00f, 1.00f, 0.31f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.31f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.00f, 1.00f, 1.00f, 0.54f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.00f, 1.00f, 1.00f, 0.31f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TabSelected]            = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
	colors[ImGuiCol_TabSelectedOverline]    = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TabDimmed]              = ImVec4(0.14f, 0.14f, 0.14f, 0.97f);
	colors[ImGuiCol_TabDimmedSelected]      = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
	colors[ImGuiCol_TabDimmedSelectedOverline]  = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_DockingPreview]         = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextLink]               = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(1.00f, 1.00f, 1.00f, 0.39f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.17f, 0.17f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.13f, 0.13f, 0.13f, 0.00f);

}

}

#endif /* SHADOW_NATIVE_IMGUI_THEME_HPP */