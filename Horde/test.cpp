#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

int main(int argc, const char** argv) {
	std::cout << std::thread::hardware_concurrency() << " threads usable" << std::endl;

	if (argc < 2) {
		std::cerr << "Must specify pipe to read" << std::endl;
		return 1;
	}
	std::cout << "Reading from: " << argv[1] << std::endl;
	std::ifstream file(argv[1]);

	if (!file) {
		std::cerr << "Can't open pipe" << std::endl;
		return 1;
	}

	std::string line;
	std::vector<std::string> jobStack;
	std::mutex mutex;

	while (std::getline(file, line)) {
		if (line == "")
			continue;

		jobStack.push_back(line);
	}

	for (int i = 0; i < std::thread::hardware_concurrency(); i++) {
		std::thread t([&jobStack, &mutex]() {
			mutex.lock();
			std::string job = jobStack.back();
			jobStack.pop_back();
			mutex.unlock();

			std::cout << job << std::endl;
		});
		t.join();
	}

	return 0;
}