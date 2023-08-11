#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "imgui.h"
#include "ppk_assert.h"
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

	commands.push_back("HELP");
	commands.push_back("CLEAR");
	commands.push_back("HISTORY");

	addLog("Welcome to Shadow Engine");
}

EditorConsole::~EditorConsole() {
	clearLog();
	for (int i = 0; i < history.size(); i++)
		free(history[i]);
}

void EditorConsole::clearLog() {
	for (int i = 0; i < items.size(); i++)
		free(items[i]);
	items.clear();
}

void EditorConsole::addLog(const char* fmt, ...) {
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, SHADOW_ARRAYSIZE(buf), fmt, args);
	buf[SHADOW_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	items.push_back(Strdup(buf));
}

void EditorConsole::onUpdate(bool* p_open) {
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(consoleName.c_str(), p_open)) {
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
		addLog("HElloooo! %d", items.size());

	if (ImGui::BeginPopup("Options")) {
		ImGui::Checkbox("Auto-Scroll", &autoScroll);
		ImGui::EndPopup();
	}
	if (ImGui::Button("Options"))
		ImGui::OpenPopup("Options");

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

	for (int i = 0; i < items.size(); i++) {
		// const char* item = items[i];
		// TODO: Possibly filter here

		const char* hello = "Hello";
		auto length = strlen(hello);
		WARN("%i", length);

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
		ImGui::Text("Test");
		// if (has_color)
		// 	ImGui::PopStyleColor();
	}

	ImGui::PopStyleVar();

	ImGui::End();
}

EditorConsoleManager::EditorConsoleManager() { }

EditorConsoleManager::~EditorConsoleManager() { }

void EditorConsoleManager::createNewConsole(std::string name) {
	EditorConsole console;
	console.consoleName = name;
	consoles.push_back(console);
}

void EditorConsoleManager::onUpdate() {
	for (int i = 0; i < consoles.size(); i++) {
		consoles[i].get().onUpdate(&showConsoles);
	}
}

}