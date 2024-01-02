#include "Debug/EditorConsole.hpp"
#include "bx/bx.h"
#include "bx/string.h"
#include "imgui.h"
#include "types.hpp"
#include "ppk_assert_impl.hpp"
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>

namespace Shadow::EditorConsole {

struct message {
	int severity;
	const char* category;
	const char* caller;
	const char* data;

	message(
		int severity,
		const char* category,
		const char* caller,
		const char* data)
		: severity(severity)
		, category(category)
		, caller(caller)
		, data(data) { }
};

static std::vector<const char*> categories;
static std::vector<message> items;

static char* Strdup(const char* s) {
	PPK_ASSERT(s);
	size_t len = strlen(s) + 1;
	void* buf = malloc(len);
	PPK_ASSERT(buf);
	return (char*)memcpy(buf, (const void*)s, len);
}

void newCategory(const char *category) {
	for (const char* existing : categories) {
		if (strcmp(existing, category) == 0) return;
	}
	categories.push_back(category);
}

void print(int severity, const char *caller, const char *category, const char *fmt, ...) {
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	std::vsnprintf(buf, SHADOW_ARRAYSIZE(buf), fmt, args);
	buf[SHADOW_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	items.push_back({ severity, category, caller, Strdup(buf)});
}

void clearLog() {
	items.clear();
}

namespace Frontend {

static bool autoscroll = true;
static bool showCallers = false;
static bool showSeverityLogLevel0 = true;
static bool showSeverityLogLevel1 = true;
static bool showSeverityLogLevel2 = true;
static std::string selectedCategory = "All";
static ImGuiTextFilter filter;

void onUpdate() {
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	ImGui::Begin("Shadow Engine Console");

	if (ImGui::SmallButton("Clear")) clearLog();

	if (ImGui::BeginPopup("Console Options")) {
		ImGui::Checkbox("Auto Scroll", &autoscroll);
		ImGui::Checkbox("Show Log Callers", &showCallers);
		ImGui::Separator();
		ImGui::Checkbox("Show info level", &showSeverityLogLevel0);
		ImGui::Checkbox("Show warn level", &showSeverityLogLevel1);
		ImGui::Checkbox("Show error level", &showSeverityLogLevel2);
		ImGui::EndPopup();
	}

	ImGui::SameLine();
	if (ImGui::SmallButton("Options")) ImGui::OpenPopup("Console Options");

	ImGui::SameLine();

	filter.Draw("Filter (inc,-exc)", 100.0f);

	if (ImGui::BeginTabBar("ConsoleSortingTabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_FittingPolicyScroll)) {
		if (ImGui::BeginTabItem("All")) {
			selectedCategory = "All";
			ImGui::EndTabItem();
		}

		for (const char* category : categories) {
			if (ImGui::BeginTabItem(category)) {
				selectedCategory = category;
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

		for (message& item : items) {
			// Filter message by category
			if (selectedCategory != "All" &&
				strcmp(item.category, selectedCategory.c_str()) != 0) continue;

			if (!showSeverityLogLevel0 && item.severity == 0) continue;
			if (!showSeverityLogLevel1 && item.severity == 1) continue;
			if (!showSeverityLogLevel2 && item.severity == 2) continue;

			if (!filter.PassFilter(item.data)) continue;

			if (item.severity == 1) ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 163, 51, 255));
			if (item.severity == 2) ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 66, 66, 255));

			char displayCategory[512] = "";
			if (selectedCategory == "All") {
				bx::strCopy(displayCategory, BX_COUNTOF(displayCategory), "[");
				bx::strCat(displayCategory, BX_COUNTOF(displayCategory), item.category);
				bx::strCat(displayCategory, BX_COUNTOF(displayCategory), "] ");
			}

			char displayCaller[512] = "";
			if (showCallers) {
				bx::strCopy(displayCaller, BX_COUNTOF(displayCaller), "{");
				bx::strCat(displayCaller, BX_COUNTOF(displayCaller), item.caller);
				bx::strCat(displayCaller, BX_COUNTOF(displayCaller), "} ");
			}

			ImGui::Text("%s%s%s", displayCategory, displayCaller, item.data);

			if (item.severity != 0) ImGui::PopStyleColor();
		}

		ImGui::PopStyleVar();

		if (autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
	}
	ImGui::EndChild();

	ImGui::End();
}

}

}