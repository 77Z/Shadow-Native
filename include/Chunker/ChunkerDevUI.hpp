#ifndef SHADOW_NATIVE_CHUNKER_CHUNKER_DEVUI_HPP
#define SHADOW_NATIVE_CHUNKER_CHUNKER_DEVUI_HPP

#include "Chunker/Chunker.hpp"
#include "imgui.h"
#include "imgui/imgui_memory_editor.h"
#include <string>

namespace Shadow {
namespace Chunker {

	class ChunkerDevUI {
	public:
		void drawUI();

	private:
		std::string chunkLocation = "menu.chunk";
		std::string fileToReadFrom = "menu.chunk/fileA.txt";

		Shadow::Chunker::FileIndex fileIndex;
		MemoryEditor memEdit;

		char openFileData[1024];
	};

}
}

#endif /* SHADOW_NATIVE_CHUNKER_CHUNKER_DEVUI_HPP */