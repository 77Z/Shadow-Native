#include "Runtime.h"

namespace Shadow {
	int Main(int argc, char** argv) {
		return Shadow::StartRuntime();
	}
}

// This is seperate so we can support Windows' wacky main function if we want to
int main(int argc, char** argv) {
	return Shadow::Main(argc, argv);
}
