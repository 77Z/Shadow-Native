#ifndef SHADOW_NATIVE_CHUNKER_HPP
#define SHADOW_NATIVE_CHUNKER_HPP

#include "Chunker.hpp.old"
#include "Core.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>

namespace Shadow::Chunker {

enum CompressionType_ : uint8_t {
	CompressionType_None = 0x00,
	CompressionType_Snappy = 0x01
};

class Chunk {
public:
	Chunk();
	~Chunk();

	CompressionType_ compression = CompressionType_None;

	MethodStatus::Enum deserializeChunkerFile(const char* chunkerFilePath);

	/// Set the compression you'd like to use before calling this
	MethodStatus::Enum serializeDirectory(const std::string& inputPath, const std::string& outputPath);

private:
	struct FileLocation {
		uint64_t offset;
		uint64_t size;
	};

	uint64_t TOCSize; // Combined size of Header + TOC
	std::unordered_map<std::string, FileLocation> locationMap;
};

}

#endif /* SHADOW_NATIVE_CHUNKER_HPP */