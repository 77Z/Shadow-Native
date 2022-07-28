#include "Runtime.h"

namespace Shadow {
	int Main(int argc, char** argv) {
		return Shadow::StartRuntime();
	}
}

int main(int argc, char** argv) {
	return Shadow::Main(argc, argv);
}
