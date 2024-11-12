#include "Debug/Logger.hpp"
#include <cstdlib>
#include <ctime>
#include <string>
#include "json_impl.hpp"

namespace Shadow {

int startCrashpad(int argc, char** argv) {

	PRINT("Shadow Engine Crash Handling enabled!!");

	std::string childCommand = std::string(argv[0]) + " ";

	for (int i = 0; i < argc; i++) {
		if (i > 1) childCommand += std::string(argv[i]) + " ";
	}

	PRINT("Child process cmd: %s", childCommand.c_str());

	int returnCode = std::system(childCommand.c_str());
	if (returnCode != 0) {
		ERROUT("Child process failed! CRASH DETECTED: %d", returnCode);

		json dumpFile;
		dumpFile["time"] = std::time(nullptr);
		dumpFile["process"] = childCommand;
		dumpFile["exitCode"] = returnCode;
		// TODO: Track process stdout here??

		std::string dumpfilePath = "/tmp/ShadowEngine-dumpfile" + std::to_string(std::time(nullptr)) + ".sedmp";

		JSON::dumpJsonToFile(dumpFile, dumpfilePath);

		// Launch Shadow Engine Crash Handler app?
		// TODO: NO ABSOLUTE PATHS
		std::system(("cd /code/axe/Shadow-Native && ./ShadowCrashHandler " + dumpfilePath).c_str());

		return 0;
	}
	
	
	PRINT("No Crashpad errors to report, exiting in peace 😴");
	
	return 0;
}

}