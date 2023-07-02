#include "Chunker/ChunkerDevUI.hpp"
#include "Chunker/Chunker.hpp"
#include "imgui.h"
#include "imgui/imgui_utils.h"
#include <string>

static std::string chunkLocation;
static std::string fileToReadFrom;

void Shadow::Chunker::drawUI() {
	ImGui::Begin("Chunker");
	ImGui::InputText("Chunk Path", &chunkLocation);
	ImGui::Button("Index Chunk");
	ImGui::InputText("File To Read From", &fileToReadFrom);
	ImGui::Button("Read File");
	ImGui::SameLine();
	ImGui::Text("(File contents shown in Memory Editor)");

	ImGui::SeparatorText("File Index");

	ImGui::End();
}