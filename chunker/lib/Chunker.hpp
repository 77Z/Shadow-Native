#ifndef SHADOW_NATIVE_CHUNKER_HPP
#define SHADOW_NATIVE_CHUNKER_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Shadow::Chunker {

enum CompressionType_ : uint8_t {
	CompressionType_None = 0x00,
	CompressionType_Snappy = 0x01
};

inline std::string CompressionTypeToString(CompressionType_ compressionType) {
	switch(compressionType) {
		case CompressionType_None: return "None";
		case CompressionType_Snappy: return "Snappy";
		default: return "?Unknown?";
	}
}

/// It's recomended to leave the contents of this struct alone
/// and at most read the filename. Instead, use this struct to
/// pass to the various Chunker methods at let those functions
/// handle working with the struct
struct Chunk {
	std::string chunkFileLocation;
	const char* filename;
	uint8_t version;
	Chunker::CompressionType_ compression;
	uint64_t headerSize;
	std::unordered_map<std::string, uint64_t> offsetMap;
	std::unordered_map<std::string, uint64_t> sizeMap;
};

// -- CHUNK CREATION -- //

int chunkDirectory(const std::string& dirPath, CompressionType_ compression, const std::string& outputPath);

// -- CHUNK READING -- //

/// Returns a Chunk struct that can be passed to readFile to
/// read the contents of a specific file.
/// chunkFileLocation is a location on disk
Chunk loadChunk(const char* chunkFileLocation);

/// Input the Chunk struct from loadChunk and the name of
/// the file to retrieve from within the Chunk. This
/// returns the contents inside an std::string, though this
/// does not mean that the contents are text, it could be
/// anything.
std::string readFile(Chunk* chunk, const char* filename);

}

#endif /* SHADOW_NATIVE_CHUNKER_HPP */
