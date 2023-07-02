#ifndef SHADOW_NATIVE_CHUNKER_CHUNKER_HPP
#define SHADOW_NATIVE_CHUNKER_CHUNKER_HPP

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Shadow {
namespace Chunker {

	enum CompressionType : uint8_t { None = 0, LZ4 = 1, Snappy = 2 };

	inline std::string CompressionTypeToString(CompressionType compressionType) {
		switch (compressionType) {
		case None:
			return "None";
		case LZ4:
			return "LZ4";
		case Snappy:
			return "Snappy";
		default:
			return "WHAT";
		}
	}

	struct FileIndex {
		const char* filename;
		Chunker::CompressionType compression;
		uint32_t internalFilesize;
		std::unordered_map<std::string, uint32_t> offsetMap;
		std::unordered_map<std::string, uint32_t> sizeMap;
	};

	int chunkFolder(std::string folderpath, CompressionType compression);
	Chunker::FileIndex indexChunk(const char* inputfile);
	std::vector<char> readFile(Chunker::FileIndex index, const char* innerFile);

}
}

#endif /* SHADOW_NATIVE_CHUNKER_CHUNKER_HPP */