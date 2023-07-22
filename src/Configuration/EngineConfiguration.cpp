#include "Configuration/EngineConfiguration.hpp"
#include "Configuration/engineConfig.sec.hpp"
#include "Debug/Logger.h"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

namespace Shadow {
namespace EngineConfiguration {

	EngineConfigFile getConfig() {
		EngineConfigFile confFile;

		std::ifstream input(engineConfigFileLoc);
		std::string line;

		while (std::getline(input, line)) {
			if (line == "")
				continue;
			size_t sepPos = line.find(":");
			std::string key = line.substr(0, sepPos);
			std::string value = line.substr(sepPos + 1);

			if (key == "lastOpenProject") {
				confFile.lastOpenProject = value;
			} else if (key == "projectBrowserWindowWidth") {
				confFile.projectBrowserWindowBounds.width = std::stoi(value);
			} else if (key == "projectBrowserWindowHeight") {
				confFile.projectBrowserWindowBounds.height = std::stoi(value);
			} else if (key == "editorWindowWidth") {
				confFile.editorWindowBounds.width = std::stoi(value);
			} else if (key == "editorWindowHeight") {
				confFile.editorWindowBounds.height = std::stoi(value);
			}
		}

		input.close();
		return confFile;
	}

	int initializeEngineConfig() {
		const std::string configDir = getConfigDir();

		PRINT("Initializing engine configuration directory: %s", configDir.c_str());

		if (std::filesystem::exists(configDir))
			return 0;

		std::filesystem::create_directory(configDir);
		std::filesystem::create_directory(configDir + "/Projects");

		std::ofstream engineConfigFile(configDir + "/engineConfig.sec");
		engineConfigFile.write(engineConfigSecData, strlen(engineConfigSecData));
		engineConfigFile.close();

		return 0;
	}

}
}