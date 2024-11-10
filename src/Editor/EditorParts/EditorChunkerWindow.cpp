#include "Editor/EditorParts/EditorParts.hpp"
#include "imgui.h"

namespace Shadow::Editor::EditorParts {

#if 0
static std::string SDinputPath = "";
static std::string SDoutputPath = "";
static bool SDcompress = true;
#endif

void chunkerWindowUpdate() {
	using namespace ImGui;

	Begin("Chunker");

	TextWrapped("Use the Shadow-Helper utility to generate chunker files");

#if 0
	TextWrapped("Output is shown in the Chunker category of the EditorConsole");

	SeparatorText("Serialize Directory");

	InputText("Input Path", &SDinputPath);
	InputText("Output Path", &SDoutputPath);
	Checkbox("Compress contents", &SDcompress);
	if (Button("Chunk")) {
		// Shadow::Chunker::Chunk chunk;
		// chunk.
	}

	SeparatorText("Deserialize Chunker File");

	// InputText("Folder to chunk", std::string *str)

	SeparatorText("Debug");
#endif

	End();
}

}