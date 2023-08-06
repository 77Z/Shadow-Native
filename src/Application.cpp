#include "Configuration/EngineConfiguration.hpp"
#include "Debug/Logger.h"
#include "DevEntryPoint.hpp"
#include "Editor/Editor.hpp"
#include "Editor/ProjectBrowser.hpp"
#include "ProductionRuntime.hpp"
#include "Runtime.h"

namespace Shadow {
int Main(int argc, char** argv) {
	int ret;

	if (EngineConfiguration::initializeEngineConfig() != 0)
		return 1;

#ifdef SHADOW_PRODUCTION_BUILD
	return Shadow::StartProductionRuntime();
#else
	// return Shadow::StartRuntime();
	// ret = Editor::startProjectBrowser();

	Editor::ProjectEntry project;
	project.path = "/home/vince/.config/Shadow/Projects/WIS";
	project.name = "WIS";
	ret = Shadow::startEditor(project);

	// ret = devEntry();
#endif

	PRINT("Goodbye from Shadow Engine");
	return ret;
}
}

// This is seperate so we can support Windows' wacky main function if we want to
int main(int argc, char** argv) { return Shadow::Main(argc, argv); }
