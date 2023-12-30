#include "Configuration/EngineConfiguration.hpp"
#include "Debug/Logger.hpp"
#include "json_impl.hpp"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

namespace Shadow::EngineConfiguration {

	int initializeEngineConfig() {
		const std::string configDir = getConfigDir();

		PRINT("Initializing engine configuration directory: %s", configDir.c_str());

		if (std::filesystem::exists(configDir))
			return 0;

		std::filesystem::create_directory(configDir);
		std::filesystem::create_directory(configDir + "/Projects");
		std::filesystem::create_directory(configDir + "/AXEProjects");

		json engineConfigFile;

		engineConfigFile["vscodeDefaultEditor"] = true;
		engineConfigFile["alternativeEditor"] = "";

		JSON::dumpJsonToBson(engineConfigFile, configDir + "/engineConfig.sec");

		return 0;
	}

}
