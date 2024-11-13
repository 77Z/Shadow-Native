#include "Debug/Logger.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <utility>
#include "json_impl.hpp"
#include "generated/autoconf.h"
#include "bx/bx.h"

namespace Shadow {

static std::pair<std::string, int> blockingExecuteCommand(const std::string& childCommand) {
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen((childCommand + " 2>&1").c_str(), "r"), pclose);

	if (!pipe) throw std::runtime_error("popen failed");

	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}

	int returnCode = pclose(pipe.release());
	return std::make_pair(result, returnCode);
}

static std::string getCoredumpLocation() {
	std::string latestFile;

#if BX_PLATFORM_LINUX
	try {
		time_t latestTime = 0;
		for (auto& file : std::filesystem::directory_iterator("/var/lib/systemd/coredump/")) {
			if (file.is_directory()) continue;
			if (!file.is_regular_file()) continue;

			struct stat fileStat;
			if (stat(file.path().c_str(), &fileStat) == 0) {
				if (fileStat.st_mtime > latestTime) {
					latestTime = fileStat.st_mtime;
					latestFile = file.path().string();
				}
			}
		}
	} catch (const std::exception& e) {
		ERROUT("Failed to get coredump: %s", e.what());
		return "";
	}
#else
#	error Not implemented!
#endif

	return latestFile;
}

int startCrashpad(int argc, char** argv) {

	PRINT("Shadow Engine Crash Handling enabled!!");

	std::string childCommand = std::string(argv[0]) + " ";

	for (int i = 0; i < argc; i++) {
		if (i > 1) childCommand += std::string(argv[i]) + " ";
	}

	PRINT("Child process cmd: %s", childCommand.c_str());

	std::pair<std::string, int> proc = blockingExecuteCommand(childCommand);
	if (proc.second != 0) {
		ERROUT("Child process failed! CRASH DETECTED");

		json dumpFile;
		dumpFile["time"]         = std::time(nullptr);
		dumpFile["process"]      = childCommand;
		dumpFile["exitCode"]     = (int)proc.second;
		dumpFile["stdout"]       = proc.first;
		dumpFile["coredumpLoc"]  = getCoredumpLocation();
		dumpFile["coredumpInfo"] = blockingExecuteCommand("coredumpctl --no-pager info").first;

		std::string dumpfilePath = "/tmp/ShadowEngine-dumpfile" + std::to_string(std::time(nullptr)) + ".sedmp";

		JSON::dumpJsonToFile(dumpFile, dumpfilePath);

		std::system(("cd " + (std::string)HORDE_PROJECT_PWD + " && ./ShadowCrashHandler " + dumpfilePath).c_str());

		return 0;
	}
	
	
	PRINT("No Crashpad errors to report, exiting in peace 😴");
	
	return 0;
}

}