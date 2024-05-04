#include "CommandCenterBackend.hpp"
#include "Debug/Logger.hpp"
#include "Editor/EditorParts/EditorParts.hpp"
#include "Keyboard.hpp"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include <string>
#include <variant>

namespace Shadow::Editor::EditorParts {

static bool commandCenterOpen = false;
static std::string commandQuery = "";
static int selectedIndex = 0;

/// Only momentarily used to grab focus on the input field.
/// Set this to true and it'll be flipped back immediately.
static bool grabFocus = false;
static bool takeAction = false;

void openCommandCenter() { commandCenterOpen = true; }
void closeCommandCenter() { commandCenterOpen = false; }
void toggleCommandCenter() {
	commandCenterOpen = !commandCenterOpen;
	if (commandCenterOpen) grabFocus = true;
}

void commandCenterInit(EditorPartsCarePackage carePackage) {
	carePackage.keyboardRef->registerKeyCallback([](KeyButton_ key, bool down, KeyModifiers_ mods) {
		if (mods == KeyModifiers_Control
			&& key == KeyButton_P
			&& down) toggleCommandCenter();
		
		if (!commandCenterOpen) return;

		if (key == KeyButton_Escape && down) closeCommandCenter();

		if (key == KeyButton_Up && down) {
			selectedIndex--;
			if (selectedIndex < 0) selectedIndex = 0;
		}

		if (key == KeyButton_Down && down) {
			selectedIndex++;
			if (selectedIndex > static_cast<int>(CommandCenter::getCommands().size()) - 1) {
				selectedIndex = static_cast<int>(CommandCenter::getCommands().size()) - 1;
			}
		}

		if (key == KeyButton_Enter && down) {
			takeAction = true;
		}
	});
}

void commandCenterUpdate() {

	if (!commandCenterOpen) return;

	ImGuiWindowFlags windowFlags
		= ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoNavInputs;

	ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->GetCenter().x, 25), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
	ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Always);

	ImGui::Begin("Command Center", nullptr, windowFlags);

	ImGui::Text("Do things in Shadow Engine...");
	if (grabFocus) {
		ImGui::SetKeyboardFocusHere();
		grabFocus = false;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(11, 7));
	// ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0.8f, 0.8f, 0.8f, 1.0f));
	ImGui::InputText("##dothings", &commandQuery, ImGuiInputTextFlags_CallbackEdit);
	// ImGui::PopStyleColor();
	ImGui::PopStyleVar();

	CommandCenter::commandList cmdList = CommandCenter::getCommands();

	int i = 0;
	for (const auto& cmd : cmdList) {
		std::string name = std::get<0>(cmd);
		std::string description = std::get<1>(cmd);
		void (*func)() = std::get<2>(cmd);

		std::string lowercaseName = name;
		std::string lowercaseQuery = commandQuery;
		std::transform(lowercaseName.begin(), lowercaseName.end(), lowercaseName.begin(), ::tolower);
		std::transform(lowercaseQuery.begin(), lowercaseQuery.end(), lowercaseQuery.begin(), ::tolower);

		if (!commandQuery.empty() && lowercaseName.find(lowercaseQuery) == std::string::npos) continue;

		std::string text = name + (selectedIndex == i ? "\n    " + description : "");

		if (ImGui::Selectable(text.c_str(), selectedIndex == i)) {
			func();
			closeCommandCenter();
		}

		if (selectedIndex == i) {
			ImGui::SetScrollHereY(0.0f);
		}

		if (takeAction && selectedIndex == i) {
			func();
			closeCommandCenter();
		}

		i++;
	}

	ImGui::End();
}

}