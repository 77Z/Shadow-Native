#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "imgui.h"
#include "imgui/imgui_memory_editor.h"
#include "ppk_assert_impl.hpp"
#include "types.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Shadow {

static char* Strdup(const char* s) {
	PPK_ASSERT(s);
	size_t len = strlen(s) + 1;
	void* buf = malloc(len);
	PPK_ASSERT(buf);
	return (char*)memcpy(buf, (const void*)s, len);
}

EditorConsole::EditorConsole() {
	memset(inputBuf, 0, sizeof(inputBuf));

	data = malloc(512);

	commands.push_back("HELP");
	commands.push_back("CLEAR");
	commands.push_back("HISTORY");

	categories.push_back("meshing");
	categories.push_back("bruz");
	addLog("All", "Welcome to Shadow Engine");
}

EditorConsole::~EditorConsole() {
	free(data);

	clearLog();
	for (int i = 0; i < history.size(); i++)
		free(history[i]);
}

void EditorConsole::clearLog() {
	for (int i = 0; i < items.size(); i++) {
		// free((char*)items[i].category);
		// free((char*)items[i].data);
		free(items[i].bindata);
	}
	items.clear();
}

void EditorConsole::addLog(const char* category, const char* fmt, ...) {
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, SHADOW_ARRAYSIZE(buf), fmt, args);
	buf[SHADOW_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	items.push_back({ category, Strdup(buf), nullptr });
}

void EditorConsole::addLogWithBinData(const char* category, void* binData, const char* fmt, ...) {
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, SHADOW_ARRAYSIZE(buf), fmt, args);
	buf[SHADOW_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	items.push_back({ category, Strdup(buf), binData });
}

void EditorConsole::onUpdate(bool* p_open) {
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Shadow Engine Console", p_open)) {
		ImGui::End();
		return;
	}

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Close Console"))
			*p_open = false;
		ImGui::EndPopup();
	}

	if (ImGui::SmallButton("clear"))
		clearLog();
	ImGui::SameLine();
	if (ImGui::SmallButton("Add Debug text"))
		addLog("meshing", "jd %d", items.size());

	ImGui::SameLine();
	if (ImGui::SmallButton("Add bindata"))
		addLogWithBinData("meshing", (void *)0x0435, "Data");

	if (ImGui::BeginPopup("Options")) {
		ImGui::Checkbox("Auto-Scroll", &autoScroll);
		ImGui::EndPopup();
	}
	if (ImGui::Button("Options"))
		ImGui::OpenPopup("Options");

	if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

		if (ImGui::BeginTabBar("ConsoleSortingTabs", ImGuiTabBarFlags_Reorderable)) {

			if (ImGui::BeginTabItem("All")) {
				currentCategory = "All";
				ImGui::EndTabItem();
			}

			for (int i = 0; i < categories.size(); i++) {
				if (ImGui::BeginTabItem(categories[i])) {
					currentCategory = categories[i];
					ImGui::EndTabItem();
				}
			}

			ImGui::EndTabBar();
		}

		for (int i = 0; i < items.size(); i++) {
			auto item = items[i];

			if (strcmp(currentCategory.c_str(), "All") != 0
				&& strcmp(item.category, currentCategory.c_str()) != 0)
				continue;

			// Normally you would store more information in your item than just a string.
			// (e.g. make Items[] an array of structure, store color/type etc.)
			// ImVec4 color;
			// bool has_color = false;
			// if (strstr(item, "[error]")) {
			// 	color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
			// 	has_color = true;
			// } else if (strncmp(item, "# ", 2) == 0) {
			// 	color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
			// 	has_color = true;
			// }
			// if (has_color)
			// 	ImGui::PushStyleColor(ImGuiCol_Text, color);
			// ImGui::TextUnformatted(item);
			// ImGui::TextUnformatted(item.category);
			ImGui::Text("[%s] %s", item.category, item.data);

			if (item.bindata != nullptr) {
				if (ImGui::TreeNode("Binary Data")) {
					edit.DrawContents(data, 512);
					ImGui::TreePop();
				}
			}
			// if (has_color)
			// 	ImGui::PopStyleColor();
		}
		ImGui::PopStyleVar();

		// Keep up at the bottom of the scroll region if we were already at the bottom at the
		// beginning of the frame. Using a scrollbar or mouse-wheel will take away from the bottom
		// edge.
		if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
	}
	ImGui::EndChild();

	ImGui::End();
}

}