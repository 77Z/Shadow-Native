#include <cstdint>
#include <cstdio>
#include <iostream>
#include "../lib/Chunker.hpp"
#include "../../include/generated/autoconf.h"
#include <bx/commandline.h>

void printVersion() {
	printf("Chunker CLI\n"
		"CLI interface for the Chunker file streaming system\n"
		"Built for Shadow Engine version %s\n"
		"Commit: %s\n"
		, SHADOW_VERSION_STRING
		, SHADOW_COMMIT_HASH
	);
}

void printHelp() {
	printVersion();

	printf(
		"\n"
		"Options:\n"
		"\n"
		"-h, --help                                      Display help and exit\n"
		"-v, --version                                   Display version info and exit\n"
		"\n"
		"  Creating Chunks:\n"
		"    --chunkDir <dir path>                       Chunk a directory into a .chunk file\n"
		"      -o, --output <output path>                Specifies output path for chunk, the\n"
		"                                                default name 'out.chunk' will be used\n"
		"                                                otherwise\n"
		"      -c, --compress                            Use snappy compression in the .chunk\n"
		"\n"
		"  Reading Chunks:\n"
		"    --chunkInfo <chunk path>                    Get info about a .chunk file\n"
		"    --readFile <chunk path> <inner file path>   Outputs the contents of a file contained\n"
		"                                                in a chunk to stdout\n"
	);
}

int main(int argc, char** argv) {

	bx::CommandLine cmdLine(argc, argv);

	if (cmdLine.hasArg('v', "version")) {
		printVersion();
		return 0;
	}

	if (cmdLine.hasArg('h', "help")) {
		printHelp();
		return 1;
	}

	bool useCompression = cmdLine.hasArg('c', "compress");

	const char* chunkDir = cmdLine.findOption("chunkDir");
	if (chunkDir != nullptr) {
		using namespace Shadow::Chunker;
		
		std::string outputPath;
		const char* userOutPath = cmdLine.findOption('o', "output");
		if (userOutPath == nullptr) outputPath = "out.chunk";
		
		int status = chunkDirectory(
			std::string(chunkDir),
			useCompression
				? CompressionType_Snappy
				: CompressionType_None,
			outputPath
		);

		if (!status) {
			std::cerr << "Unable to chunk directory" << std::endl;
			return 1;
		}

		return 0;
	}

	const char* chunkInfo = cmdLine.findOption("chunkInfo");
	if (chunkInfo != nullptr) {
		using namespace Shadow::Chunker;

		Chunk chunk = loadChunk(chunkInfo);

		printf("Chunk information for %s\n\n", chunkInfo);
		printf("Compression: %s\n", CompressionTypeToString(chunk.compression).c_str());
		printf("Header size: %lu\n", chunk.headerSize);
		printf("Chunk version: %u\n", chunk.version);
		printf("Files contained:\n");
		for (auto& item : chunk.sizeMap)
			printf("    %s [size: %lu, offset: %lu]\n",
				item.first.c_str(),
				item.second,
				chunk.offsetMap[item.first]
			);

		return 0;
	}

	const char* chunkToRead = cmdLine.findOption("readFile", 2);
	if (chunkToRead != nullptr) {
		using namespace Shadow::Chunker;

		Chunk chunk = loadChunk(chunkToRead);
		std::string fileContent = readFile(&chunk, cmdLine.get(3));

		std::cout << fileContent;

		return 0;
	}

	printHelp();
	return 1;
}
