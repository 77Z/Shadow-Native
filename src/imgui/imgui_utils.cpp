#include "imgui/imgui_utils.hpp"
#include "imgui.h"
#include "imgui_internal.h"

struct InputTextCallback_UserData {
	std::string* Str;
	ImGuiInputTextCallback ChainCallback;
	void* ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data) {
	InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
		// Resize string callback
		// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we
		// need to set them back to what we want.
		std::string* str = user_data->Str;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	} else if (user_data->ChainCallback) {
		// Forward to user callback, if any
		data->UserData = user_data->ChainCallbackUserData;
		return user_data->ChainCallback(data);
	}
	return 0;
}

bool ImGui::InputText(const char* label, std::string* str, ImGuiInputTextFlags flags,
	ImGuiInputTextCallback callback, void* user_data) {
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return InputText(
		label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

bool ImGui::myBeginMenuBar() {
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;
	if (!(window->Flags & ImGuiWindowFlags_MenuBar))
		return false;

	IM_ASSERT(!window->DC.MenuBarAppending);
	BeginGroup(); // Backup position on layer 0 // FIXME: Misleading to use a group for that
				  // backup/restore
	PushID("##menubar");

	// We don't clip with current window clipping rectangle as it is already set to the area below.
	// However we clip with window full rect. We remove 1 worth of rounding to Max.x to that text in
	// long menus and small windows don't tend to display over the lower-right rounded area, which
	// looks particularly glitchy.
	ImRect bar_rect = window->MenuBarRect();
	/* ImRect clip_rect(IM_ROUND(bar_rect.Min.x + window->WindowBorderSize),
		IM_ROUND(bar_rect.Min.y + window->WindowBorderSize),
		IM_ROUND(ImMax(bar_rect.Min.x,
			bar_rect.Max.x - ImMax(window->WindowRounding, window->WindowBorderSize))),
		IM_ROUND(bar_rect.Max.y)); */

	/* ImRect clip_rect(IM_ROUND(ImMax(window->Pos.x,
						 bar_rect.Min.x + window->WindowBorderSize + window->Pos.x - 10.0f)),
		IM_ROUND(bar_rect.Min.y + window->WindowBorderSize + window->Pos.y),
		IM_ROUND(ImMax(bar_rect.Min.x + window->Pos.x,
			bar_rect.Max.x - ImMax(window->WindowRounding, window->WindowBorderSize))),
		IM_ROUND(bar_rect.Max.y + window->Pos.y)); */

	ImRect clip_rect(bar_rect.Min.x, bar_rect.Min.y, 600, bar_rect.Max.y);
	clip_rect.ClipWith(window->OuterRectClipped);
	PushClipRect(clip_rect.Min, clip_rect.Max, false);
	// GetForegroundDrawList()->AddRect(clip_rect.Min, clip_rect.Max, IM_COL32(255, 0, 0, 255));
	// GetForegroundDrawList()->AddRect(bar_rect.Min, bar_rect.Max, IM_COL32(255, 0, 0, 255));

	// We overwrite CursorMaxPos because BeginGroup sets it to CursorPos (essentially the .EmitItem
	// hack in EndMenuBar() would need something analogous here, maybe a BeginGroupEx() with flags).
	window->DC.CursorPos = window->DC.CursorMaxPos = ImVec2(
		bar_rect.Min.x + window->DC.MenuBarOffset.x, bar_rect.Min.y + window->DC.MenuBarOffset.y);
	window->DC.LayoutType = ImGuiLayoutType_Horizontal;
	window->DC.IsSameLine = false;
	window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;
	window->DC.MenuBarAppending = true;
	AlignTextToFramePadding();
	return true;
}

bool ImGui::ToggleButton(const char* label, bool* v) {
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems) return false;

	ImGuiStyle style = GetStyle();

	bool ret = false;

	if (*v) {

		if (Button(label)) {
			*v = !*v;
			ret = true;
		}

	} else {
		PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_ButtonActive]);
		PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_ButtonActive]);
		if (Button(label)) {
			*v = !*v;
			ret = true;
		}
		PopStyleColor(2);
	}

	return ret;
}