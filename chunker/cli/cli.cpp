#include <iostream>
#include <string.h>
#include "Chunker.hpp"

int main(int argc, char** argv) {

	int status = 0;

	if (argc == 1) {
		std::cout << R"(Shadow Engine Chunker CLI

c: [UNSUPPORTED] chunk a folder using LZ4 compression
s: [RECOMMENDED] chunk a folder using Snappy compression
u:               chunk a folder using no compression
		./chunker s ./Resources/mydata.chunk mydata.chunk

l: list the contents of a chunk
		./chunker l mydata.chunk
r: dump the contents of a file in a chunk to stdout
		./chunker r mydata.chunk internaldir/internalfile.txt
)";
		return 1;
	}

	if (strcmp(argv[1], "c") == 0) {
		std::cout << "Compiling folder [LZ4]: " << argv[2] << std::endl;
		status = Chunker::chunkFolder(argv[2], Chunker::CompressionType_LZ4, argv[3]);
	} else if (strcmp(argv[1], "s") == 0) {
		std::cout << "Compiling folder [SNAPPY]: " << argv[2] << std::endl;
		status = Chunker::chunkFolder(argv[2], Chunker::CompressionType_Snappy, argv[3]);
	} else if (strcmp(argv[1], "u") == 0) {
		std::cout << "Compiling folder [RAW]: " << argv[2] << std::endl;
		status = Chunker::chunkFolder(argv[2], Chunker::CompressionType_None, argv[3]);
	} else if (strcmp(argv[1], "r") == 0) {
		auto index = Chunker::indexChunk(argv[2]);

		auto data = Chunker::readFile(index, argv[3]);

		std::cout << data;

		// No need to convert
		// std::cout << std::string(data.data(), data.size()) << std::endl;
	} else if (strcmp(argv[1], "l") == 0) {
		auto index = Chunker::indexChunk(argv[2]);
		for (auto item: index.offsetMap)
			std::cout << item.first << std::endl;
	} else {
		std::cerr << "Unknown command: " << argv[1] << std::endl;
		return 1;
	}

	return status;
}
