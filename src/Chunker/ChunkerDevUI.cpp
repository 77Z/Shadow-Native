#include "Chunker/ChunkerDevUI.hpp"
#include "Chunker/Chunker.hpp"
#include "Debug/Logger.h"
#include "imgui.h"
#include "imgui/imgui_memory_editor.h"
#include "imgui/imgui_utils.h"
#include <cstring>
#include <string>

void Shadow::Chunker::ChunkerDevUI::drawUI() {
	ImGui::Begin("Chunker");
	ImGui::InputText("Chunk Path", &chunkLocation);

	if (ImGui::Button("Index Chunk")) {
		fileIndex = Chunker::indexChunk(chunkLocation.c_str());
	}

	ImGui::InputText("File To Read From", &fileToReadFrom);
	if (ImGui::Button("Read File")) {
		std::string data = Chunker::readFile(fileIndex, fileToReadFrom.c_str());
		memcpy(openFileData, data.data(), data.size());

		// std::string convData(data.data(), data.size());
		// PRINT("%s", convData.c_str());
	}
	ImGui::SameLine();
	ImGui::Text("(File contents displayed in a Memory Editor)");

	ImGui::SeparatorText("File Index");

	for (auto item : fileIndex.offsetMap) {
		ImGui::Text("%s", item.first.c_str());
	}

	ImGui::End();

	memEdit.DrawWindow("Data from open Chunker file", &openFileData, sizeof(openFileData));
}