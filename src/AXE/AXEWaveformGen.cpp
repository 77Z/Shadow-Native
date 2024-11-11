#include "AXEWaveformGen.hpp"
#include "AXEGlobalSettingsWindow.hpp"
#include "AXEJobSystem.hpp"
#include "Configuration/EngineConfiguration.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "json_impl.hpp"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

#define EC_THIS "WaveformGen"

namespace Shadow::AXE {

void cacheWaveforms() {
	EC_NEWCAT(EC_THIS);

	std::string globalLibraryPath = EngineConfiguration::getConfigDir() + "/AXEProjects/GlobalLibrary";
	std::string cachePath = EngineConfiguration::getConfigDir() + "/AXECachedData";
	PRINT("%s", globalLibraryPath.c_str());

	JobSystem::submitJob("Cache waveform data", [globalLibraryPath, cachePath]() {
		EC_PRINT(EC_THIS, "Generating waveforms as job");

		if (!std::filesystem::exists(cachePath)) {
			EC_PRINT(EC_THIS, "/AXECachedData dir doesn't exist, making now...");
			std::filesystem::create_directory(cachePath);
		}

		for (const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator(globalLibraryPath)) {
			if (file.is_regular_file()/*  && file.path().extension() == ".wav" */) {
				std::ifstream inFile(file.path(), std::ios::binary);
				std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(inFile), {});
				std::size_t checksum = std::hash<std::string>{}(std::string(buffer.begin(), buffer.end()));

				std::string uniqueName = file.path().string().substr(globalLibraryPath.length() + 1);
				std::replace(uniqueName.begin(), uniqueName.end(), '/', '-');

				std::string preProcessedWaveformFileName = uniqueName + ".json";
				std::string waveformFileName = uniqueName + ".AXEwf";

				EC_PRINT(EC_THIS, "Operating on file %s", uniqueName.c_str());
				EC_PRINT(EC_THIS, "    - HASH: %s", std::to_string(checksum).c_str());

				std::system(std::string("audiowaveform -z " + std::to_string(/* getGlobalSettings()->samplesPerPixel */ 100) + " -i " + file.path().string() + " -o " + cachePath + "/" + preProcessedWaveformFileName).c_str());

				// Post-process data
				// We inject arbitrary props at this point

				std::ifstream f(cachePath + "/" + preProcessedWaveformFileName);
				json j = json::parse(f);
				f.close();
				j["audioHash"] = std::to_string(checksum);

				JSON::dumpJsonToBson(j, cachePath + "/" + waveformFileName);

				std::filesystem::remove(cachePath + "/" + preProcessedWaveformFileName);

			}
		}

		return true;
	});
}

}