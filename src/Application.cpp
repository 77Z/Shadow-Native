#include "Editor/ProjectBrowser.hpp"
#include "ProductionRuntime.hpp"
#include "Runtime.h"

namespace Shadow {
int Main(int argc, char** argv) {
#ifdef SHADOW_PRODUCTION_BUILD
	return Shadow::StartProductionRuntime();
#else
	// return Shadow::StartRuntime();
	return Editor::startProjectBrowser();
#endif
}
}

// This is seperate so we can support Windows' wacky main function if we want to
int main(int argc, char** argv) { return Shadow::Main(argc, argv); }
