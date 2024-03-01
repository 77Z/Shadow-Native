#include "Editor/EditorParts/EditorParts.hpp"
#include "bx/debug.h"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
// #include "" // Chunker
#include <cstdint>
#include <string>

namespace Shadow::Editor::EditorParts {

static std::string SDinputPath = "";
static std::string SDoutputPath = "";
static bool SDcompress = true;

struct buf {
	uint8_t id;
	float data;
};

struct __attribute__((__packed__)) packed_buf {
	uint8_t id;
	float data;
};

void chunkerWindowUpdate() {
	ImGui::Begin("Chunker");

	ImGui::TextWrapped("Output is shown in the Chunker category of the EditorConsole");

	ImGui::SeparatorText("Serialize Directory");

	ImGui::InputText("Input Path", &SDinputPath);
	ImGui::InputText("Output Path", &SDoutputPath);
	ImGui::Checkbox("Compress contents", &SDcompress);
	if (ImGui::Button("Chunk")) {
		// Shadow::Chunker::Chunk chunk;
		// chunk.
	}

	ImGui::SeparatorText("Deserialize Chunker File");

	// ImGui::InputText("Folder to chunk", std::string *str)

	ImGui::SeparatorText("Debug");

	if (ImGui::Button("Alloc and break struct")) {
		buf buffer;
		packed_buf packed_buffer;
		
		buffer.id = 0xFF;
		buffer.data = 3.232;

		packed_buffer.id = 0xEC;
		packed_buffer.data = 3.14159265;

		bx::debugBreak();
	}

	ImGui::End();
}

}