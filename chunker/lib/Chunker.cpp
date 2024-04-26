#include "Chunker.hpp"
#include "Core.hpp"
// #include "Debug/EditorConsole.hpp"
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

#define CHUNKER_FORMAT_VERSION 0x03

namespace Shadow::Chunker {


MethodStatus::Enum Chunk::deserializeChunkerFile(const char* chunkerFilePath) {
	return MethodStatus::Success;
}

MethodStatus::Enum Chunk::serializeDirectory(const std::string& inputPath, const std::string& outputPath) {

	std::ifstream targetDir(inputPath);
	if (!targetDir) {
		// EC_ERROUT("Chunker", "Failed to open dir: %s", inputPath.c_str());
		return MethodStatus::Failure;
	}
	targetDir.close();

	const unsigned char nullchar = 0x00;

	std::fstream outputFile(outputPath);

	// * Magic header
	const char* magic = "SHADOW CHUNK"; // 12 bytes
	outputFile.write(magic, strlen(magic));

	// * Version number
	const uint8_t versionNumber = CHUNKER_FORMAT_VERSION;
	outputFile.write((char*)&versionNumber, sizeof(versionNumber));

	// * Compression type
	outputFile.write((char*)&compression, sizeof(compression));

	// * Internal filesize template
	const uint64_t ifTemplate = 0x0000000000000000;
	outputFile.write((char*)&ifTemplate, sizeof(ifTemplate));


	uint64_t offset = 0;
	for (const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator(inputPath)) {
		std::string filename = file.path().string();
		// EC_PRINT("Chunker", "%s", filename.c_str());
	}

	outputFile.close();

	return MethodStatus::Success;
}

}
