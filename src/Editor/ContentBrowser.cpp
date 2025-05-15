#include "Editor/ContentBrowser.hpp"
#include "Debug/Logger.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Project.hpp"
#include "Util.hpp"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "bx/platform.h"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "imgui_internal.h"
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>
#include "../AXE/ShadowIcons.hpp"

namespace ImGui::ContentBrowser {

bool fileNode(Shadow::ContentBrowser::fileEntry file, ImTextureID icon) {

	const char* label = file.name.c_str();

	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size(100, 130);
	ImRect boundingBox(pos, ImVec2(pos.x + size.x, pos.y + size.y));

	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ItemSize(size);
	if (!ItemAdd(boundingBox, id))
		return false;

	// Render
	bool hovered, held;
	bool pressed = ButtonBehavior(boundingBox, id, &hovered, &held);

	if (hovered && ImGui::BeginTooltip()) {
		ImGui::PushStyleColor(ImGuiCol_Text, file.fileTypeColor);
		ImGui::TextUnformatted(file.fileTypePrettyName.c_str());
		ImGui::PopStyleColor();
		ImGui::Separator();
		ImGui::Text("%s", label);
		ImGui::EndTooltip();
	}

	if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		OpenPopup("fileNode ctx menu");
		PRINT("BRuz");
	}

	if (ImGui::BeginPopup("fileNode ctx menu")) {
		if (ImGui::Selectable("Open")) {
			ImGui::EndPopup();
			return true;
		}
		ImGui::Selectable("Delete");
		ImGui::Selectable("Rename");

		ImGui::EndPopup();
	}

	const ImU32 color
		= GetColorU32(hovered ? IM_COL32(34, 34, 34, 255) : IM_COL32(20, 20, 20, 255));
	RenderNavHighlight(boundingBox, id);
	RenderFrame(boundingBox.Min, boundingBox.Max, color, true, 3.0f);

	RenderTextClipped(ImVec2(boundingBox.Min.x, boundingBox.Min.y + 100),
		ImVec2(boundingBox.Max.x, boundingBox.Max.y + 100), label, NULL, &label_size, ImVec2(0, 0),
		&boundingBox);

	window->DrawList->AddImage(
		icon, boundingBox.Min, ImVec2(boundingBox.Max.x, boundingBox.Max.y - 30));

	// GetForegroundDrawList()->AddRect(boundingBox.Min, boundingBox.Max, IM_COL32(255, 0, 0, 255));

	ImGui::SameLine();
	// return pressed;

	return hovered && IsMouseDoubleClicked(0);
}

}

namespace Shadow {

ContentBrowser::ContentBrowser() {
	loadDir(&activeFileIndex, "/");
}

ContentBrowser::~ContentBrowser() { }

void ContentBrowser::unload() {
	for (const auto& item : fileTypeMap) {
		bgfx::destroy(item.second);
	}
}

static std::string currentDir = "/";
std::string ContentBrowser::getCurrentDir() { return currentDir; }
void ContentBrowser::reloadCurrentDir() { loadDir(&activeFileIndex, getCurrentDir()); }

void ContentBrowser::loadDir(std::vector<fileEntry>* indexToWrite, std::string dir) {
	std::string dirToRead = Editor::getCurrentProjectPath() + "/Content/" + dir;
#if __cplusplus > BX_LANGUAGE_CPP17
	if (dir.ends_with("/") && dir.length() > 1)
		dir.pop_back();
#else
	if (dir[dir.length() - 1] == '/' && dir.length() > 1)
		dir.pop_back();
#endif
	currentDir = Util::removeDupeSlashes(dir);

	indexToWrite->clear();

	for (const auto& file : std::filesystem::directory_iterator(dirToRead)) {
		fileEntry fe;
		fe.isFile = file.is_regular_file();
		fe.isFolder = file.is_directory();
		fe.fullpath = file.path().string();
		fe.name = fe.fullpath.substr(fe.fullpath.find_last_of("/") + 1, fe.fullpath.length());

		if (file.is_directory()) {
			fe.fileType = fileTypes_Folder;
			fe.fileTypePrettyName = "Directory";
			fe.fileTypeColor = IM_COL32(255, 255, 255, 255);
		} else {
			std::string path = file.path().string();
			std::string fileExt = path.substr(path.find_last_of(".") + 1);

			if		(fileExt == "cpp")		{ fe.fileType = fileTypes_CppSource; fe.fileTypePrettyName = "C++ Source File"; fe.fileTypeColor = IM_COL32(56, 76, 255, 255); }
			else if	(fileExt == "hpp")		{ fe.fileType = fileTypes_HppSource; fe.fileTypePrettyName = "C++ Header File"; fe.fileTypeColor = IM_COL32(56, 76, 255, 255); }
			else if	(fileExt == "c")		{ fe.fileType = fileTypes_CSource; fe.fileTypePrettyName = "C Source File"; fe.fileTypeColor = IM_COL32(56, 76, 255, 255); }
			else if	(fileExt == "scene")	{ fe.fileType = fileTypes_ShadowEngineScene; fe.fileTypePrettyName = "Shadow Engine Scene"; fe.fileTypeColor = IM_COL32(255, 184, 0, 255); }
			else if	(fileExt == "ktx")		{ fe.fileType = fileTypes_KTXTexture; fe.fileTypePrettyName = "Khronos Texture"; fe.fileTypeColor = IM_COL32(106, 0, 255, 255); }
			else if	(fileExt == "png")		{ fe.fileType = fileTypes_PNGTexture; fe.fileTypePrettyName = "PNG Texture"; fe.fileTypeColor = IM_COL32(106, 0, 255, 255); }
			else if (fileExt == "prefab")	{ fe.fileType = fileTypes_Prefab; fe.fileTypePrettyName = "Shadow Engine Prefab"; fe.fileTypeColor = IM_COL32(6, 141, 195, 255); }
			else { fe.fileType = fileTypes_Other; fe.fileTypePrettyName = "? Unknown file type ?"; fe.fileTypeColor = IM_COL32(113, 113, 113, 255); }
		}

		indexToWrite->push_back(fe);
	}
}

static std::string userinputPath = "";
static bool userinputtingPath = false;
static bool once = false;

void ContentBrowser::drawPathNavigationRail() {
	std::string currentDir = Util::removeDupeSlashes(getCurrentDir());
	std::string formattedDir = currentDir.substr(1, currentDir.length());
	std::vector<std::string> pathSplit = Util::splitString(formattedDir, '/');

	ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(17, 17, 17, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(17, 17, 17, 255));

	if (userinputtingPath) {
		if (!once) {
			ImGui::SetKeyboardFocusHere();
			once = true;
		}
		ImGui::InputText("##manualpathinput", &userinputPath);
		// ImGui::SameLine();
		if (ImGui::IsItemDeactivatedAfterEdit()) {
			userinputtingPath = false;
			if (userinputPath.empty()) {
				loadDir(&activeFileIndex, "/");
			} else {
				std::string path = Editor::getCurrentProjectPath() + "/Content/" + userinputPath;

				if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
					loadDir(&activeFileIndex, userinputPath);
				else
					PRINT("Notification here: path doesn't exist or not dir");
			}
		}
	} else {
		once = false;
		if (ImGui::Button("CONTENT")) {
			loadDir(&activeFileIndex, "/");
		}

		ImGui::SameLine();
		ImGui::Text(SHADOW_ICON_CHEVRON_RIGHT);
		ImGui::SameLine();

		// Each directory in the loop stacks onto this string allowing for
		// navigation anywhere along the current tree quickly
		std::string builder = "/";

		for (const std::string& dir : pathSplit) {
			builder += dir + "/";
			if (ImGui::Button((dir + "##blah").c_str()))
				loadDir(&activeFileIndex, builder);
			ImGui::SameLine();
			ImGui::Text(SHADOW_ICON_CHEVRON_RIGHT);
			ImGui::SameLine();
		}
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 0, 0, 0));
		if (ImGui::Button("##manualpath",
				ImVec2(ImGui::GetWindowSize().x - ImGui::GetCursorPosX() - 10, 20 /* TODO: Scale factor here */ ))) {
			userinputPath = currentDir;
			userinputtingPath = true;
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
		}
		ImGui::PopStyleColor();
	}

	ImGui::PopStyleColor(3);
}

static std::string newDirPath = "";

void ContentBrowser::onUpdate() {
	ImGui::SetNextWindowSize(ImVec2(1600, 200), ImGuiCond_FirstUseEver);
	ImGui::Begin("Content Browser");
	
	if (ImGui::BeginPopupContextItem("newfilepopup")) {
		if (ImGui::Selectable("Directory", false, ImGuiSelectableFlags_DontClosePopups)) {
			ImGui::OpenPopup("NewDirPopup");
		}
		if (ImGui::Selectable("C++ Source File"))
			PRINT("New C++ File");
		if (ImGui::Selectable("C++ Header File"))
			PRINT("New c++ Header");
		ImGui::Separator();
		if (ImGui::Selectable("C++ Class"))
			PRINT("CLASS");
		ImGui::Separator();
		if (ImGui::Selectable("Prefab Actor"))
			PRINT("PREFAB");
		if (ImGui::Selectable("Texture"))
			PRINT("Texture");



		if (ImGui::BeginPopupModal("NewDirPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("dirname", &newDirPath);
			if (ImGui::Button("Create")) {
				std::filesystem::create_directory(Util::removeDupeSlashes(Editor::getCurrentProjectPath() + "/Content/" + getCurrentDir() + "/" + newDirPath));
				reloadCurrentDir();
				newDirPath.clear();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}


		ImGui::EndPopup();
	}
	
	if (ImGui::Button("+ Add"))
		ImGui::OpenPopup("newfilepopup");

	ImGui::SameLine();

	ImGui::BeginDisabled(getCurrentDir() == "/");
	if (ImGui::Button(SHADOW_ICON_ARROW_UP)) {
		std::string currentDir = getCurrentDir();
		std::string newDir = currentDir.substr(0, currentDir.find_last_of("/"));
		loadDir(&activeFileIndex, newDir.empty() ? "/" : newDir);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal) && ImGui::BeginTooltip()) {
		ImGui::Text("Ascend file tree");
		ImGui::EndTooltip();
	}
	ImGui::EndDisabled();

	ImGui::SameLine();

	if (ImGui::Button(SHADOW_ICON_REFRESH)) {
		reloadCurrentDir();
	}

	ImGui::SameLine();

	drawPathNavigationRail();

	// Prints the current directory, useful for debugging the content browser.
	// ImGui::Text("%s", getCurrentDir().c_str());

	// ImGui::Image(fileIcon, ImVec2(70, 70));

	// ImGui::ContentBrowser::fileNode("Main.cpp", ImGui::toId(cppIcon, 0, 0));
	// ImGui::ContentBrowser::fileNode("Other.cpp", ImGui::toId(cppIcon, 0, 0));
	// ImGui::ContentBrowser::fileNode("Other.hpp", ImGui::toId(hppIcon, 0, 0));

	for (int i = 0; i < activeFileIndex.size(); i++) {
		fileEntry file = activeFileIndex[i];

		// This switch is executed every frame. Checking file extentions
		// every frame would be hell. That's checked during indexing.
		switch (file.fileType) {

			case fileTypes_Folder: {
				if (ImGui::ContentBrowser::fileNode(file, ImGui::toId(fileTypeMap["folder"], 0, 0))) {
					loadDir(&activeFileIndex, getCurrentDir() + "/" + file.name);
				}
				break;
			}
			case fileTypes_CSource:
			case fileTypes_CppSource: {
				if (ImGui::ContentBrowser::fileNode(file, ImGui::toId(fileTypeMap["cpp"], 0, 0))) {
					PRINT("Open Code Editor for file: %s", file.fullpath.c_str());
				}
				break;
			}
			case fileTypes_HppSource: {
				if (ImGui::ContentBrowser::fileNode(file, ImGui::toId(fileTypeMap["hpp"], 0, 0))) {
					PRINT("Open Code Editor for file: %s", file.fullpath.c_str());
				}
				break;
			}
			case fileTypes_ShadowEngineScene: {
				if (ImGui::ContentBrowser::fileNode(file, ImGui::toId(fileTypeMap["scene"], 0, 0))) {
					PRINT("load scene into editor: %s", file.fullpath.c_str());
					loadScene(file.fullpath.c_str());
				}
				break;
			}
			case fileTypes_KTXTexture: {
				ImGui::ContentBrowser::fileNode(file, ImGui::toId(fileTypeMap["ktx"], 0, 0));
				break;
			}
			case fileTypes_PNGTexture: {
				ImGui::ContentBrowser::fileNode(file, ImGui::toId(fileTypeMap["png"], 0, 0));
				break;
			}
			case fileTypes_Prefab: {
				ImGui::ContentBrowser::fileNode(file, ImGui::toId(fileTypeMap["prefab"], 0, 0));
				break;
			};
			case fileTypes_Other: {
				ImGui::ContentBrowser::fileNode(file, ImGui::toId(fileTypeMap["other"], 0, 0));
				break;
			}
				break;
			}
	}

	ImGui::End();
}

}