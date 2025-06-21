#include "Chunker.hpp"
#include "snappy.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define CHUNKER_FORMAT_VERSION 0x03

namespace fs = std::filesystem;

namespace Shadow::Chunker {

/// Returns 0 on failure, 1 on success
int chunkFiles(const std::vector<fs::path>& inputFiles, const fs::path outputFilePath, CompressionType_ compression) {

	std::fstream outputFile(outputFilePath, std::ios::out | std::ios::binary);
	if (!outputFile.is_open()) return 0;

	// * Magic header identifier
	const char* magic = "SHADOW CHUNK"; // 12 bytes
	outputFile.write(magic, strlen(magic));

	// * Chunker version number
	constexpr uint8_t versionNumber = CHUNKER_FORMAT_VERSION;
	outputFile.write((char*)&versionNumber, sizeof(versionNumber));

	// * Compression Type
	outputFile.write((char*)&compression, sizeof(compression));

	// * Header size
	constexpr uint64_t headerTemplate = 0; // 64 bit : 8 bytes
	outputFile.write((char*)&headerTemplate, sizeof(headerTemplate));

	// * Table of Contents
	uint64_t offset = 0;
	for (const auto& inputFile : inputFiles) {
		// Loading file data just to get its size
		std::ifstream iteratedFile(inputFile);
		std::stringstream ss;
		ss << iteratedFile.rdbuf();
		std::string fileData = ss.str();
		std::string applicableFileData;

		if (compression == CompressionType_Snappy) {
			snappy::Compress(fileData.data(), fileData.size(), &applicableFileData);
		} else {
			applicableFileData = fileData;
		}

		// Write the filename, stating the size of the filename string beforehand
		// as an uint32_t
		std::string filename = inputFile.filename().string();
		uint32_t filenameSize = filename.size();
		outputFile.write((char*)&filenameSize, sizeof(filenameSize));
		outputFile.write(filename.c_str(), filenameSize);

		// Write file offset as an u64
		outputFile.write((char*)&offset, sizeof(offset));

		// Write file size as an u64
		uint64_t encodedFilesize = (uint64_t) applicableFileData.size();
		outputFile.write((char*)&encodedFilesize, sizeof(encodedFilesize));

		// Add current file size to offset to shift offset
		offset = offset + encodedFilesize;

		iteratedFile.close();
	}

	// Overwrite the 8 bytes at 0x14, the headerSize, that stores the size of
	// the Chunk metadata and the Table of Contents.
	// Seek to end
	outputFile.seekg(0, std::ios::end);
	uint64_t headerSizeToWrite = outputFile.tellg();
	outputFile.seekg(14);
	outputFile.write((char*)&headerSizeToWrite, sizeof(headerSizeToWrite));

	// Second iteration over all the files to add their data to the chunk file
	for (const auto& inputFile : inputFiles) {
		std::ifstream iteratedFile(inputFile);

		// Retrieve file contents
		std::stringstream ss;
		ss << iteratedFile.rdbuf();
		std::string fileData = ss.str();
		std::string applicableFileData;

		if (compression == CompressionType_Snappy) {
			snappy::Compress(fileData.data(), fileData.size(), &applicableFileData);
		} else {
			applicableFileData = fileData;
		}

		outputFile.seekg(0, std::ios::end);
		outputFile << applicableFileData;

		iteratedFile.close();
	}

	outputFile.close();

	return 1;
}

/// Returns 0 on failure, 1 on success
int chunkDirectory(
	const std::string& dirPath, CompressionType_ compression, const std::string& outputPath) {

	std::ifstream operatingDir(dirPath);
	if (!operatingDir) {
		std::cerr << "Can't open directory" << std::endl;
		return 0;
	}
	operatingDir.close();

	std::fstream outputChunkFile(outputPath, std::ios::out);

	std::cout << "Writing to file " << outputPath << std::endl;

	// * Magic header identifier
	const char* magic = "SHADOW CHUNK"; // 12 bytes
	outputChunkFile.write(magic, strlen(magic));

	// * Chunker version number
	const uint8_t versionNumber = CHUNKER_FORMAT_VERSION;
	outputChunkFile.write((char*)&versionNumber, sizeof(versionNumber));

	// * Compression Type
	outputChunkFile.write((char*)&compression, sizeof(compression));

	// * Header size
	const uint64_t headerTemplate = 0x0000000000000000; // 64-bit
	outputChunkFile.write((char*)&headerTemplate, sizeof(headerTemplate));

	// * Table of Contents
	uint64_t offset = 0;
	// Iterate through every file in the provided directory
	for (const std::filesystem::directory_entry& file :
		std::filesystem::recursive_directory_iterator(dirPath)) {
		std::string filename = file.path().string();

		// Loading the file data only to get its size
		std::ifstream iteratedFile(filename);
		std::stringstream ss;
		ss << iteratedFile.rdbuf();
		std::string fileData = ss.str();
		std::string applicableFileData;

		if (compression == CompressionType_Snappy) {
			snappy::Compress(fileData.data(), fileData.size(), &applicableFileData);
		} else {
			applicableFileData = fileData;
		}

		// Write the filename, stating the size of the filename string beforehand
		// as a uint32_t
		uint32_t filenameSize = filename.size();
		outputChunkFile.write((char*)&filenameSize, sizeof(filenameSize));
		outputChunkFile.write(filename.c_str(), filenameSize);

		// Write file offset as a uint64_t
		outputChunkFile.write((char*)&offset, sizeof(offset));

		// Write file size as a uint64_t
		uint64_t encodedFilesize = (uint64_t)applicableFileData.size();
		outputChunkFile.write((char*)&encodedFilesize, sizeof(encodedFilesize));

		// Add current file size to offset to shift offset
		offset = offset + encodedFilesize;

		iteratedFile.close();
	}

	// Overwrite the 8 bytes at 0x14, the headerSize, that stores the size of
	// the Chunk metadata and the Table of Contents.
	// Seek to end
	outputChunkFile.seekg(0, std::ios::end);
	uint64_t headerSizeToWrite = outputChunkFile.tellg();
	outputChunkFile.seekg(14);
	outputChunkFile.write((char*)&headerSizeToWrite, sizeof(headerSizeToWrite));

	// 2nd iteration over all the files to add their data to the Chunk file
	for (const std::filesystem::directory_entry& file :
		std::filesystem::recursive_directory_iterator(dirPath)) {
		std::string filename = file.path().string();

		// Retrieve file contents
		std::ifstream iteratedFile(filename);
		std::stringstream ss;
		ss << iteratedFile.rdbuf();
		std::string fileData = ss.str();
		std::string applicableFileData;

		if (compression == CompressionType_Snappy) {
			snappy::Compress(fileData.data(), fileData.size(), &applicableFileData);
		} else {
			applicableFileData = fileData;
		}

		outputChunkFile.seekg(0, std::ios::end);
		// Using << because data can contain null chars
		outputChunkFile << applicableFileData;

		iteratedFile.close();
	}

	outputChunkFile.close();

	return 1;
}

// TODO: convert to const std::string???
Chunk loadChunk(const char* chunkFileLocation) {
	Chunk chunk;

	chunk.chunkFileLocation = (std::string)chunkFileLocation;

	std::ifstream file(chunkFileLocation, std::ios::binary | std::ios::in);
	if (!file) {
		std::cerr << "Cannot open chunker file for reading" << std::endl;
		abort();
	}

	// Header verification
	file.seekg(0);
	char headerVerificationBuf[13];
	file.read(headerVerificationBuf, 12);
	headerVerificationBuf[12] = '\0';
	if (strcmp(headerVerificationBuf, "SHADOW CHUNK") != 0) {
		std::cerr << "Header doesn't match! Is this a chunker file??" << std::endl;
		exit(1);
	}

	// Version verification
	file.seekg(12);
	uint8_t versionBuf = 0x00;
	file.read((char*)&versionBuf, 1);
	if (versionBuf != CHUNKER_FORMAT_VERSION) {
		std::cerr << "Wrong chunk version!" << std::endl;
		exit(1);
	}
	chunk.version = versionBuf;

	chunk.filename = chunkFileLocation;

	// Detect compression type
	file.seekg(13);
	CompressionType_ compressionBuf = CompressionType_None;
	file.read((char*)&compressionBuf, 1);
	chunk.compression = compressionBuf;

	// Read headerSize
	file.seekg(14);
	uint64_t headerSizeBuf = 0;
	file.read((char*)&headerSizeBuf, sizeof(headerSizeBuf));
	chunk.headerSize = headerSizeBuf;

	file.seekg(22);

	// TODO: fix later
	while (file.tellg() < chunk.headerSize) {
		// Read filename size
		uint32_t filenameSizeBuffer = 0;
		file.read((char*)&filenameSizeBuffer, sizeof(filenameSizeBuffer));

		// Read filename
		std::string filename;
		filename.resize(filenameSizeBuffer);
		file.read(&filename[0], filenameSizeBuffer);

		// Read file offset
		uint64_t fileOffsetBuf = 0;
		file.read((char*)&fileOffsetBuf, sizeof(fileOffsetBuf));

		// Read file size
		uint64_t fileSizeBuf = 0;
		file.read((char*)&fileSizeBuf, sizeof(fileSizeBuf));

		// add to index
		chunk.offsetMap[filename] = fileOffsetBuf;
		chunk.sizeMap[filename] = fileSizeBuf;
	}

	return chunk;
}

// TODO: Should the user of the method pass a pointer to a file that this
// method can then utilize? It would stop the repeat opening and closing of
// chunker files.
std::string readFile(Chunk* chunk, const char* filename) {

	std::ifstream file(chunk->chunkFileLocation);

	std::vector<char> returnBuf(chunk->sizeMap[filename]);

	file.seekg(chunk->offsetMap[filename] + chunk->headerSize);
	file.read(returnBuf.data(), returnBuf.size());

	std::string result;

	if (chunk->compression == CompressionType_Snappy) {
		std::string toBeDecompressed = std::string(returnBuf.data(), returnBuf.size());
		snappy::Uncompress(toBeDecompressed.data(), toBeDecompressed.size(), &result);
	} else {
		result = std::string(returnBuf.data(), returnBuf.size());
	}

	return result;
}
}
