//
// Created by vince on 6/19/25.
//

#include "../lib/Chunker.hpp"
#include <filesystem>
#include <iostream>

#include <vector>

namespace fs = std::filesystem;

static void printUsage(const char* name) {
	std::cerr << "Usage: " << name << " file1 file2 file3... -o output.sec " << std::endl;
}

int main(const int argc, char* argv[]) {
	if (argc == 1) {
		printUsage(argv[0]);
		return 1;
	}

	std::vector<fs::path> inputFiles;
	std::string outputFile = "";

	// populate inputs arr
	for (int i = 1; i < argc; i++) {
		inputFiles.emplace_back(argv[i]);
	}

	// find output file
	for (size_t i = 0; i < inputFiles.size(); i++) {
		if (inputFiles[i] == "-o") {
			outputFile = inputFiles[i + 1];
			inputFiles.erase(inputFiles.begin() + i);
			inputFiles.erase(inputFiles.begin() + i);
			break;
		}
	}


	if (inputFiles.empty() && outputFile.empty()) {
		printUsage(argv[0]);
		return 1;
	}

	std::cout << "input files" << std::endl;
	for (const auto& input : inputFiles) {
		std::cout << input.string() << std::endl;
	}
	std::cout << std::endl;
	std::cout << "output file" << std::endl;
	std::cout << outputFile << std::endl;

	int res = Shadow::Chunker::chunkFiles(inputFiles, outputFile, Shadow::Chunker::CompressionType_Snappy);

	if (res) return 0;

	std::cerr << "Failed to create chunk" << std::endl;
	return 1;
}