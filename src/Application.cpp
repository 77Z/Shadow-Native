#include "AXE/AXEEditor.hpp"
#include "AXE/AXEProjectBrowser.hpp"
#include "Configuration/EngineConfiguration.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "DevEntryPoint.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Project.hpp"
#include "Editor/ProjectBrowser.hpp"
#include "ModelViewer.hpp"
#include "ProductionRuntime/RuntimeBootstrapper.hpp"
#include "Util.hpp"
#include "json_impl.hpp"
#include "ppk_assert_impl.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <vector>
#include "generated/autoconf.h"
#include <Editor/SlimEditor.hpp>

namespace Shadow {
int Main(int argc, char** argv) {
	int ret = 0;

	PPK_ASSERT(Editor::getCurrentProjectName().empty());

	if (EngineConfiguration::initializeEngineConfig() != 0)
		return 1;

	InitBXFilesystem();

#if CONFIG_SHADOW_PRODUCTION_BUILD
	return Shadow::StartProductionRuntime();
#else

	EC_PRINT("All", "Welcome to Shadow Engine");

	if (argc > 1) {
		if (strcmp(argv[1], "axe") == 0) {
			ret = Shadow::AXE::startAXEProjectBrowser();
		} else if (strcmp(argv[1], "axeEditor") == 0) {
			ret = Shadow::AXE::startAXEEditor("/home/vince/.config/Shadow/AXEProjects/testsong.axe");
		} else if (strcmp(argv[1], "axeEditorWithProject") == 0) {
			ret = Shadow::AXE::startAXEEditor(std::string(argv[2]));
		} else if (strcmp(argv[1], "editorwis") == 0) {
			ret = Shadow::startEditor({ "WIS", "/home/vince/.config/Shadow/Projects/WIS" });
		} else if (strcmp(argv[1], "dev") == 0) {
			ret = devEntry();
		} else if (strcmp(argv[1], "sceneDecode") == 0) {
			json sceneInput = JSON::readBsonFile(argv[2]);
			std::ofstream outfile(argv[3]);
			outfile << sceneInput.dump(2);
			outfile.close();
		} else if (strcmp(argv[1], "sceneEncode") == 0) {
			WARN("Encoding %s to %s", argv[2], argv[3]);
			std::ifstream infile(argv[2]);
			std::vector<uint8_t> bson = json::to_bson(json::parse(infile));
			std::ofstream outfile(argv[3]);
			for (size_t i = 0; i < bson.size(); i++) {
				outfile << bson[i];
			}
			outfile.close();
		} else if (strcmp(argv[1], "ModelViewer") == 0) {
			ModelViewer mdlview((std::string(argv[2])));
		} else if (strcmp(argv[1], "SlimEditor") == 0) {
			Editor::startSlimEditor(std::string(argv[2]));
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
