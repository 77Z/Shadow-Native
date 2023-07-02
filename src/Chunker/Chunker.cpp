// Copyright (c) 77Z 2022-2023
// Most of this code will make no sense if you dont look
// at the diagram: "Chunker File Format.png" SO GO LOOK!

#include "Chunker/Chunker.hpp"
#include "Logger.h"
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <snappy.h>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace Shadow {

Chunker::FileIndex Chunker::indexChunk(const char* inputfile) {
	std::ifstream file(inputfile, std::ios::binary | std::ios::in);
	if (!file)
		throw std::runtime_error("Unable to open file");

	// Make sure header is correct
	file.seekg(0);
	char headerVerificationBuffer[7];
	file.read(headerVerificationBuffer, 6);
	headerVerificationBuffer[6] = '\0';
	if (strcmp(headerVerificationBuffer, "SHADOW") != 0)
		throw std::runtime_error("Header doesn't match, this isn't a Chunker file!");

	Chunker::FileIndex fileIndex;
	fileIndex.filename = inputfile;

	// Detect what kind of compression is used
	file.seekg(6);
	char compressionchar[1];
	file.read(compressionchar, 1);
	Chunker::CompressionType compression = (Chunker::CompressionType)compressionchar[0];
	PRINT("Using compression type: %s", CompressionTypeToString(compression).c_str());
	fileIndex.compression = compression;

	// Get "Internal Filesize" (see diagram)
	// Internal Filesize is stored at 0x00000007, 4 bytes long, and is uint32_t
	file.seekg(7);
	char buffer[4];
	file.read(buffer, 4);
	uint32_t internalFilesize = *(uint32_t*)&buffer; // Pointer Sorcery
	PRINT("Internal Filesize: %i", internalFilesize);
	fileIndex.internalFilesize = internalFilesize;

	// Jump to 11 to read the table of contents
	file.seekg(11);

	PRINT("Indexing files...");
	while (file.tellg() < internalFilesize) {

		char filenameSizeBuffer[4];
		file.read(filenameSizeBuffer, 4);
		uint32_t filenameSize = *(uint32_t*)&filenameSizeBuffer; // Pointer Sorcery
		std::string filename;
		filename.resize(filenameSize);
		file.read(&filename[0], filenameSize);

		char internalFileOffsetBuffer[4];
		file.read(internalFileOffsetBuffer, 4);
		uint32_t internalFileOffset = *(uint32_t*)&internalFileOffsetBuffer;

		// Yeah I suck at naming things
		char internalFileSpaceBuffer[4];
		file.read(internalFileSpaceBuffer, 4);
		uint32_t internalFileSpace = *(uint32_t*)&internalFileSpaceBuffer;

		PRINT("Filename: %s, Offset: %i, Size: %i", filename.c_str(), internalFileOffset,
			internalFileSpace);

		fileIndex.offsetMap[filename] = internalFileOffset;
		fileIndex.sizeMap[filename] = internalFileSpace;
	}

	return fileIndex;
}

std::string Chunker::readFile(Chunker::FileIndex fileIndex, const char* innerFile) {
	std::ifstream file(fileIndex.filename, std::ios::binary | std::ios::in);
	if (!file)
		throw std::runtime_error("Unable to open file");

	// Make sure header is correct
	file.seekg(0);
	char headerVerificationBuffer[7];
	file.read(headerVerificationBuffer, 6);
	headerVerificationBuffer[6] = '\0';
	if (strcmp(headerVerificationBuffer, "SHADOW") != 0)
		throw std::runtime_error("Header doesn't match, this isn't a Chunker file!");

	PRINT("Attempting to read %s", innerFile);

	std::vector<char> retBuffer(fileIndex.sizeMap[innerFile]);

	file.seekg(fileIndex.offsetMap[innerFile] + fileIndex.internalFilesize);
	file.read(retBuffer.data(), retBuffer.size());

	std::string result;

	if (fileIndex.compression == CompressionType::Snappy) {
		PRINT("Snappy compression detected, decompressing...");
		std::string toBeDecompressed = std::string(retBuffer.data(), retBuffer.size());
		snappy::Uncompress(toBeDecompressed.data(), toBeDecompressed.size(), &result);
	} else {
		result = std::string(retBuffer.data(), retBuffer.size());
	}

	return result;
}
}
