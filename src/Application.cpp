#include "AXE/AXEProjectBrowser.hpp"
#include "Configuration/EngineConfiguration.hpp"
#include "Debug/Logger.hpp"
#include "DevEntryPoint.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Project.hpp"
#include "Editor/ProjectBrowser.hpp"
#include "ProductionRuntime.hpp"
#include "Runtime.hpp"
#include "bx/bx.h"
#include "ppk_assert.h"
#include <cstring>

namespace Shadow {
int Main(int argc, char** argv) {
	BX_UNUSED(argc);

	int ret = 0;

	PPK_ASSERT(Editor::getCurrentProjectName().empty());

	if (EngineConfiguration::initializeEngineConfig() != 0)
		return 1;

#ifdef SHADOW_PRODUCTION_BUILD
	return Shadow::StartProductionRuntime();
#else

	if (argc > 1) {
		if (strcmp(argv[1], "axe") == 0) {
			ret = Shadow::AXE::startAXEProjectBrowser();
		}
	} else {
		// return Shadow::StartRuntime();

		// ret = Editor::startProjectBrowser();

		Editor::ProjectEntry project;
		project.path = "/home/vince/.config/Shadow/Projects/WIS";
		project.name = "WIS";
		ret = Shadow::startEditor(project);

		// ret = devEntry();
	}
#endif

	PRINT("Goodbye from Shadow Engine");
	return ret;
}
}

// This is seperate so we can support Windows' wacky main function if we want to
int main(int argc, char** argv) { return Shadow::Main(argc, argv); }
