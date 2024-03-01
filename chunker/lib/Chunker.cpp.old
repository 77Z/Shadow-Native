// Copyright (c) 77Z 2022-2023
// Most of this code will make no sense if you dont look
// at the diagram: "Chunker File Format.png" SO GO LOOK!

#include "Chunker.hpp"
#include "lz4.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <snappy.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../include/Debug/Logger.hpp"

// TODO: This is chunker, should every issue be fatal??
static void fatal(const char* message) {
	ERROUT("{CHUNKER FATAL} %s", message);
	abort();
}

int Chunker::chunkFolder(std::string folderpath, Chunker::CompressionType_ compression, std::string outputFilePath) {
	std::ifstream folderToChunk(folderpath);
	if (!folderToChunk) {
		std::cerr << "Unable to open given folder" << std::endl;
		return 1;
	}

	unsigned char nullchar = 0x00;

	std::fstream outfile(outputFilePath, std::ios::trunc | std::ios::out | std::ios::in | std::ios::binary);

	// Write header
	const char* header = "SHADOW";
	outfile.write(header, strlen(header));

	// Write type of compression used, stored as an unsigned 8-bit number (1 byte
	// in size)
	outfile.write((char*)&compression, sizeof(compression));

	// Write chunk filesize template
	outfile.write((char*)&nullchar, sizeof(nullchar));
	outfile.write((char*)&nullchar, sizeof(nullchar));
	outfile.write((char*)&nullchar, sizeof(nullchar));
	outfile.write((char*)&nullchar, sizeof(nullchar));

	uint32_t offset = 0;

	for (const auto& file : std::filesystem::recursive_directory_iterator(folderpath)) {
		std::string fileNameStr = file.path().string();
		std::cout << "Found file " << fileNameStr << std::endl;

		// Get file contents (we are not writing the actual data yet!)
		std::ifstream iteratedFile(fileNameStr);
		std::stringstream ss;
		ss << iteratedFile.rdbuf();
		std::string fileData = ss.str();
		std::string applicableFileData;

		if (compression == CompressionType_Snappy) {
			snappy::Compress(fileData.data(), fileData.size(), &applicableFileData);
		} else {
			applicableFileData = fileData;
		}

		// We loaded the file to get its size

		// Write filename (Terminates at \0)
		// const char* filename = fileNameStr.c_str();
		// outfile.write(filename, strlen(filename));
		// outfile.write((char*)&nullchar, sizeof(nullchar));

		// Write filename, stating the size of the string beforehand, as a uint32_t
		// This would be a size_t, but I struggle to read those for some reason
		uint32_t filenameSize = fileNameStr.size();
		std::cout << filenameSize << std::endl;
		outfile.write((char*)&filenameSize, sizeof(filenameSize));
		outfile.write(fileNameStr.c_str(), filenameSize);

		// Write offset as an unsigned 32-bit number (4 bytes in size)
		outfile.write((char*)&offset, sizeof(offset));

		// Write filesize as an unsigned 32-bit number (4 bytes in size)
		uint32_t encodedFilesize = (uint32_t)applicableFileData.size();
		outfile.write((char*)&encodedFilesize, sizeof(encodedFilesize));

		// Add current filesize to the offset
		offset = offset + encodedFilesize;

		// Close file for now
		iteratedFile.close();
	}

	std::cout << std::endl;

	// Overwrite the 4 bytes at 0x00000007 that stores the size
	// of the header + TOC

	std::cout << "Overwriting internal filesize data" << std::endl;

	// Seek to end to get file size
	outfile.seekg(0, std::ios::end);
	uint32_t offrsizetowrite = outfile.tellg();

	outfile.seekg(7);
	outfile.write((char*)&offrsizetowrite, sizeof(offrsizetowrite));

	// Use stage2 file handle if possible using file.seekg(0, std::ios::end)

	std::cout << std::endl;

	for (const auto& file : std::filesystem::recursive_directory_iterator(folderpath)) {
		// Second loop to actually add the data

		std::string fileNameStr = file.path().string();
		std::cout << "Adding file data to chunk: " << fileNameStr << std::endl;

		// Get file contents
		std::ifstream iteratedFile(fileNameStr);
		std::stringstream ss;
		ss << iteratedFile.rdbuf();
		std::string fileData = ss.str();
		std::string applicableFileData;

		if (compression == CompressionType_Snappy) {
			snappy::Compress(fileData.data(), fileData.size(), &applicableFileData);
		} else {
			applicableFileData = fileData;
		}

		outfile.seekg(0, std::ios::end);
		// Use << because data can contain nullchars
		outfile << applicableFileData;

		iteratedFile.close();
	}

	outfile.close();

	folderToChunk.close();

	return 0;
}

Chunker::FileIndex Chunker::indexChunk(const char* inputfile) {
	std::ifstream file(inputfile, std::ios::binary | std::ios::in);
	if (!file)
		fatal("Unable to open file");

	// Make sure header is correct
	file.seekg(0);
	char headerVerificationBuffer[7];
	file.read(headerVerificationBuffer, 6);
	headerVerificationBuffer[6] = '\0';
	if (strcmp(headerVerificationBuffer, "SHADOW") != 0)
		fatal("Header doesn't match, this isn't a Chunker file!");

	Chunker::FileIndex fileIndex;
	fileIndex.filename = inputfile;

	// Detect what kind of compression is used
	file.seekg(6);
	char compressionchar[1];
	file.read(compressionchar, 1);
	Chunker::CompressionType_ compression = (Chunker::CompressionType_)compressionchar[0];
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
		fatal("Unable to open file");

	// Make sure header is correct
	file.seekg(0);
	char headerVerificationBuffer[7];
	file.read(headerVerificationBuffer, 6);
	headerVerificationBuffer[6] = '\0';
	if (strcmp(headerVerificationBuffer, "SHADOW") != 0)
		fatal("Header doesn't match, this isn't a Chunker file!");

	PRINT("Attempting to read %s", innerFile);

	std::vector<char> retBuffer(fileIndex.sizeMap[innerFile]);

	file.seekg(fileIndex.offsetMap[innerFile] + fileIndex.internalFilesize);
	file.read(retBuffer.data(), retBuffer.size());

	std::string result;

	if (fileIndex.compression == CompressionType_Snappy) {
		PRINT("Snappy compression detected, decompressing...");
		std::string toBeDecompressed = std::string(retBuffer.data(), retBuffer.size());
		snappy::Uncompress(toBeDecompressed.data(), toBeDecompressed.size(), &result);
	} else {
		result = std::string(retBuffer.data(), retBuffer.size());
	}

	return result;
}