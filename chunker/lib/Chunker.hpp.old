#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Chunker {

	enum CompressionType_ : uint8_t {
		CompressionType_None = 0,
		CompressionType_LZ4 = 1,
		CompressionType_Snappy = 2
	};

	inline std::string CompressionTypeToString(CompressionType_ compressionType) {
		switch(compressionType) {
			case CompressionType_None: return "None";
			case CompressionType_LZ4: return "LZ4";
			case CompressionType_Snappy: return "Snappy";
			default: return "WHAT";
		}
	}

	struct FileIndex {
		const char* filename;
		Chunker::CompressionType_ compression;
		uint32_t internalFilesize;
		std::unordered_map<std::string, uint32_t> offsetMap;
		std::unordered_map<std::string, uint32_t> sizeMap;
	};

	int chunkFolder(std::string folderpath, CompressionType_ compression, std::string outputFilePath);
	Chunker::FileIndex indexChunk(const char* inputfile);
	std::string readFile(Chunker::FileIndex index, const char* innerFile);

}
