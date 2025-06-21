//
// Created by vince on 6/20/25.
//

#include "../lib/Chunker.hpp"

#include <iomanip>
#include <iostream>

#include <sstream>

std::string format_bytes(unsigned long long bytes) {
	if (bytes == 0) return "0 B";

	const char* suffixes[] = {"B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB"};
	int suffix_index = 0;
	double count = static_cast<double>(bytes);

	// Keep dividing by 1024 until the number is small enough
	while (count >= 1024 && suffix_index < 6) {
		count /= 1024;
		suffix_index++;
	}

	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2) << count << " " << suffixes[suffix_index];
	return oss.str();
}

int main(const int argc, char** argv) {

	if (argc != 2) {
		std::cerr << "usage: " << argv[0] << " <path to chunker file>" << std::endl;
		return 1;
	}

	// Needs a more graceful way to bailout if it's not a chunker file.
	auto chunk = Shadow::Chunker::loadChunk(argv[1]);

	std::cout << "chunk file: " << argv[1] << std::endl;
	if (chunk.compression == Shadow::Chunker::CompressionType_Snappy)
		std::cout << "using SNAPPY compression" << std::endl;
	else
		std::cout << "using NO compression" << std::endl;

	std::cout << "Contents:" << std::endl;

	for (auto& item : chunk.sizeMap) {
		printf("    %s [%ssize: %s]\n",
			item.first.c_str(),
			chunk.compression == Shadow::Chunker::CompressionType_Snappy ? "compressed " : "",
			format_bytes(item.second).c_str()
		);
	}

	return 0;
}