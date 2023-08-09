#include "Editor/ContentBrowser.hpp"
#include "Debug/Logger.h"
#include "Editor/Project.hpp"
#include "Util.h"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "bx/platform.h"
#include "imgui.h"
#include "imgui/imgui_utils.h"
#include "imgui_internal.h"
#include <filesystem>
#include <string>
#include <vector>

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
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(56, 76, 255, 255));
		ImGui::TextUnformatted("C++ Source File");
		ImGui::PopStyleColor();
		ImGui::Separator();
		ImGui::Text("%s", label);
		ImGui::EndTooltip();
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

	// GetForegroundDrawList()->AddRect(
	// 	boundingBox.Min, boundingBox.Max, IM_COL32(255, 0, 0, 255));

	ImGui::SameLine();
	// return pressed;

	return hovered && IsMouseDoubleClicked(0);
}

}

namespace Shadow {

ContentBrowser::ContentBrowser() {
	// const bgfx::Memory* mem = bgfx::alloc(270000);
	// unsigned char* texbuf = (unsigned char*)mem->data;

	// bx::memSet(texbuf, 0xff, mem->size);

	// fileIcon = bgfx::createTexture2D(
	// 	300, 300, false, 1, bgfx::TextureFormat::RGB8, BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, mem);

	cppIcon = loadTexture("./fileIcon.png");
	hppIcon = loadTexture("./hppIcon.png");
	folderIcon = loadTexture("./folderIcon.png");

	loadDir(&activeFileIndex, "/");
}

ContentBrowser::~ContentBrowser() { }

void ContentBrowser::unload() {
	bgfx::destroy(cppIcon);
	bgfx::destroy(hppIcon);
	bgfx::destroy(folderIcon);
}

static std::string currentDir = "/";
std::string ContentBrowser::getCurrentDir() { return currentDir; }

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

		/* if (file.is_directory()) {
			fe.fileType = fileTypes::Folder;
		} */

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
			if (std::filesystem::exists(
					Editor::getCurrentProjectPath() + "/Content/" + userinputPath))
				loadDir(&activeFileIndex, userinputPath);
			else
				PRINT("Notification");
		}
	} else {
		once = false;
		if (ImGui::Button("CONTENT")) {
			loadDir(&activeFileIndex, "/");
		}

		ImGui::SameLine();
		ImGui::Text(">");
		ImGui::SameLine();

		// Each directory in the loop stacks onto this string allowing for
		// navigation anywhere along the current tree quickly
		std::string builder = "/";

		for (const std::string& dir : pathSplit) {
			builder += dir + "/";
			if (ImGui::Button((dir + "##blah").c_str()))
				loadDir(&activeFileIndex, builder);
			ImGui::SameLine();
			ImGui::Text(">");
			ImGui::SameLine();
		}
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 0, 0, 0));
		if (ImGui::Button("##manualpath",
				ImVec2(ImGui::GetWindowSize().x - ImGui::GetCursorPosX() - 10, 20))) {
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

void ContentBrowser::onUpdate() {
	ImGui::SetNextWindowSize(ImVec2(1600, 200), ImGuiCond_FirstUseEver);
	ImGui::Begin("Content Browser");

	if (ImGui::BeginPopupContextItem("newfilepopup")) {
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
		ImGui::EndPopup();
	}

	if (ImGui::Button("+ Add"))
		ImGui::OpenPopup("newfilepopup");

	ImGui::SameLine();

	ImGui::BeginDisabled(getCurrentDir() == "/");
	if (ImGui::Button("^")) {
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

	if (ImGui::Button("Reload")) {
		loadDir(&activeFileIndex, getCurrentDir());
	}

	ImGui::SameLine();

	drawPathNavigationRail();

	ImGui::Text("%s", getCurrentDir().c_str());

	// ImGui::Image(fileIcon, ImVec2(70, 70));

	// ImGui::ContentBrowser::fileNode("Main.cpp", ImGui::toId(cppIcon, 0, 0));
	// ImGui::ContentBrowser::fileNode("Other.cpp", ImGui::toId(cppIcon, 0, 0));
	// ImGui::ContentBrowser::fileNode("Other.hpp", ImGui::toId(hppIcon, 0, 0));

	for (int i = 0; i < activeFileIndex.size(); i++) {
		fileEntry file = activeFileIndex[i];

		if (file.isFolder) {
			if (ImGui::ContentBrowser::fileNode(file, ImGui::toId(folderIcon, 0, 0))) {
				loadDir(&activeFileIndex, getCurrentDir() + "/" + file.name);
			}
		} else {
			ImGui::ContentBrowser::fileNode(file, ImGui::toId(cppIcon, 0, 0));
		}
	}

	ImGui::End();
}

}