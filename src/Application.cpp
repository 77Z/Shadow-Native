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

#if BX_PLATFORM_WINDOWS
#	include <Windows.h>
#	include <shellapi.h>
#endif

// Forward Decls
namespace Shadow::AXE {
int startAXEAuthenticationWindow();
}
namespace Shadow {
int startCrashpad(int argc, char** argv);
}

namespace Shadow {
int Main(const std::vector<std::string>& args) {
	int ret = 0;

	PPK_ASSERT(Editor::getCurrentProjectName().empty());

	if (EngineConfiguration::initializeEngineConfig() != 0)
		return 1;

	InitBXFilesystem();

#if CONFIG_SHADOW_PRODUCTION_BUILD
	return Shadow::StartProductionRuntime();
#else

	EC_PRINT("All", "Welcome to Shadow Engine");

	if (args.size() > 1) {
		if (args[1] == "axe") {
			ret = Shadow::AXE::startAXEProjectBrowser(args);
		} else if (args[1] == "axeEditor") {
			ret = Shadow::AXE::startAXEEditor("/home/vince/.config/Shadow/AXEProjects/testsong.axe");
		} else if (args[1] == "axeEditorWithProject") {
			ret = Shadow::AXE::startAXEEditor(args[2]);
		} else if (args[1] == "auth") {
			ret = Shadow::AXE::startAXEAuthenticationWindow();
		} else if (args[1] == "editorwis") {
			ret = Shadow::startEditor({ "WIS", "/home/vince/.config/Shadow/Projects/WIS" });
		} else if (args[1] == "dev") {
			ret = devEntry();
		} else if (args[1] == "sceneDecode") {
			json sceneInput = JSON::readBsonFile(args[2]);
			std::ofstream outfile(args[3]);
			outfile << sceneInput.dump(2);
			outfile.close();
		} else if (args[1] == "sceneEncode") {
			WARN("Encoding %s to %s", args[2].c_str(), args[3].c_str());
			std::ifstream infile(args[2]);
			std::vector<uint8_t> bson = json::to_bson(json::parse(infile));
			std::ofstream outfile(args[3]);
			for (size_t i = 0; i < bson.size(); i++) {
				outfile << bson[i];
			}
			outfile.close();
		} else if (args[1] == "ModelViewer") {
			ModelViewer mdlview(args[2]);
		} else if (args[1] == "SlimEditor") {
			Editor::startSlimEditor(args[2]);
		} else if (args[1] == "Crashpad") {
			ERROUT("Crashpad unsupported in this version!");
			ret = 1;
			// ret = startCrashpad(argc, argv);
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

#if BX_PLATFORM_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// Is there a better way to hide this?
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	LPWSTR cmdLine = GetCommandLineW();
	int argc;
	LPWSTR* argv = CommandLineToArgvW(cmdLine, &argc);
	std::vector<std::string> args;

	for (int i = 0; i < argc; i++) {
		std::wstring ws(argv[i]);
		args.emplace_back(std::string(ws.begin(), ws.end()));
	}

	return Shadow::Main(args);
}
#else
int main(int argc, char** argv) {
	std::vector<std::string> args;

	for (int i = 0; i < argc; i++) {
		args.emplace_back(std::string(argv[i]));
	}

	return Shadow::Main(args);
}
#endif
