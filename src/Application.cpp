#include "AXE/AXEEditor.hpp"
#include "AXE/AXEProjectBrowser.hpp"
#include "Configuration/EngineConfiguration.hpp"
#include "Debug/Logger.hpp"
#include "DevEntryPoint.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Project.hpp"
#include "Editor/ProjectBrowser.hpp"
#include "ProductionRuntime/RuntimeBootstrapper.hpp"
#include "Runtime.hpp"
#include "Util.hpp"
#include "ppk_assert_impl.hpp"
#include <cstring>
#include "generated/autoconf.h"

namespace Shadow {
int Main(int argc, char** argv) {
	int ret = 0;

	PPK_ASSERT(Editor::getCurrentProjectName().empty());

	if (EngineConfiguration::initializeEngineConfig() != 0)
		return 1;

	InitBXFilesystem();

#if CONFIG_SHADOW_PRODUCTION_BUILD
	return Shadow::StartProductionRuntime();
#else\

	if (argc > 1) {
		if (strcmp(argv[1], "axe") == 0) {
			ret = Shadow::AXE::startAXEProjectBrowser();
		} else if (strcmp(argv[1], "axeEditor") == 0) {
			ret = Shadow::AXE::startAXEEditor(
				{ "Bruz", "/home/vince/.config/Shadow/AXEProjects/bruz" });
		} else if (strcmp(argv[1], "editorwis") == 0) {
			ret = Shadow::startEditor({ "WIS", "/home/vince/.config/Shadow/Projects/WIS" });
		} else if (strcmp(argv[1], "dev") == 0) {
			ret = devEntry();
		}
	} else {
		// return Shadow::StartRuntime();

		ret = Editor::startProjectBrowser();

		// ret = Shadow::startEditor({ "WIS", "/home/vince/.config/Shadow/Projects/WIS" });
		// ret = Shadow::startEditor({ "WIS", "C:/Users/r2d2f/AppData/Roaming/ShadowEngine/Projects/WIS" });

		// ret = devEntry();
	}
#endif

	ShutdownBXFilesytem();
	PRINT("Goodbye from Shadow Engine");
	return ret;
}
}

// This is seperate so we can support Windows' wacky main function if we want to
int main(int argc, char** argv) { return Shadow::Main(argc, argv); }
