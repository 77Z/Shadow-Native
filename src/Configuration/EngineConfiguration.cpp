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
		std::filesystem::create_directory(configDir + "/AXEProjects/GlobalLibrary");

		json engineConfigFile;

		engineConfigFile["vscodeDefaultEditor"] = true;
		engineConfigFile["alternativeEditor"] = "";

		JSON::dumpJsonToBson(engineConfigFile, configDir + "/engineConfig.sec");

		return 0;
	}

	static std::string gblUserName = "?? Unknown User ??";

	void setUserName(const std::string& username) {
		gblUserName = username;
	}

	const std::string getUserName() {
		return gblUserName;
	}


	// A scale factor value that shadow should scale all dpi-aware pixel values by. Obviously
	// different windows can have different scale factors (and in the case of GLFW, different scale
	// factors per x and y), but it's just easier this way.
	static float gblScaleFactor = 1.0f;
	void setGlobalScaleFactor(const float sf) { gblScaleFactor = sf; }
	float getGlobalScaleFactor() { return gblScaleFactor; }

}
