#ifndef SHADOW_NATIVE_IMGUI_UTILS_HPP
#define SHADOW_NATIVE_IMGUI_UTILS_HPP

#include <bgfx/bgfx.h>
#include <cstdarg>
#include <imgui.h>
#include <string>

namespace ImGui {

#define IMGUI_FLAGS_NONE UINT8_C(0x00)
#define IMGUI_FLAGS_ALPHA_BLEND UINT8_C(0x01)

bool myBeginMenuBar();

inline void TextCenter(const char* text, ...) {
	va_list vaList;
	va_start(vaList, text);

	float font_size = ImGui::GetFontSize() * strlen(text) / 2;
	ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size + (font_size / 2));

	ImGui::TextV(text, vaList);

	va_end(vaList);
}

inline bool MouseOverArea() {
	return false || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered()
		|| ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}

bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0,
	ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);

inline ImTextureID toId(bgfx::TextureHandle _handle, uint8_t _flags, uint8_t _mip) {
	union {
		struct {
			bgfx::TextureHandle handle;
			uint8_t flags;
			uint8_t mip;
		} s;
		ImTextureID id;
	} tex;
	tex.s.handle = _handle;
	tex.s.flags = _flags;
	tex.s.mip = _mip;
	return tex.id;
}

// Helper function for passing bgfx::TextureHandle to ImGui::Image.
inline void Image(bgfx::TextureHandle _handle, uint8_t _flags, uint8_t _mip, const ImVec2& _size,
	const ImVec2& _uv0 = ImVec2(0.0f, 0.0f), const ImVec2& _uv1 = ImVec2(1.0f, 1.0f),
	const ImVec4& _tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
	const ImVec4& _borderCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f)) {
	Image(toId(_handle, _flags, _mip), _size, _uv0, _uv1, _tintCol, _borderCol);
}

// Helper function for passing bgfx::TextureHandle to ImGui::Image.
inline void Image(bgfx::TextureHandle _handle, const ImVec2& _size,
	const ImVec2& _uv0 = ImVec2(0.0f, 0.0f), const ImVec2& _uv1 = ImVec2(1.0f, 1.0f),
	const ImVec4& _tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
	const ImVec4& _borderCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f)) {
	Image(_handle, IMGUI_FLAGS_ALPHA_BLEND, 0, _size, _uv0, _uv1, _tintCol, _borderCol);
}

// Helper function for passing bgfx::TextureHandle to ImGui::ImageButton.
inline bool ImageButton(bgfx::TextureHandle _handle, uint8_t _flags, uint8_t _mip,
	const ImVec2& _size, const ImVec2& _uv0 = ImVec2(0.0f, 0.0f),
	const ImVec2& _uv1 = ImVec2(1.0f, 1.0f), const ImVec4& _bgCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
	const ImVec4& _tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f)) {
	return ImageButton("image", toId(_handle, _flags, _mip), _size, _uv0, _uv1, _bgCol, _tintCol);
}

// Helper function for passing bgfx::TextureHandle to ImGui::ImageButton.
inline bool ImageButton(bgfx::TextureHandle _handle, const ImVec2& _size,
	const ImVec2& _uv0 = ImVec2(0.0f, 0.0f), const ImVec2& _uv1 = ImVec2(1.0f, 1.0f),
	const ImVec4& _bgCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
	const ImVec4& _tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f)) {
	return ImageButton(_handle, IMGUI_FLAGS_ALPHA_BLEND, 0, _size, _uv0, _uv1, _bgCol, _tintCol);
}

// Spinners
// https://github.com/ocornut/imgui/issues/1901

bool Spinner(const char* label, float radius, int thickness, const ImU32& color);

}

#endif /* SHADOW_NATIVE_IMGUI_UTILS_HPP */