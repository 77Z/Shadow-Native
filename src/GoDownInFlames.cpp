#include "GoDownInFlames.hpp"
#include <cstdlib>
#include <filesystem>
#include <string>
#include "Debug/Logger.hpp"
#include "bx/bx.h"
#include "generated/autoconf.h"

#if BX_PLATFORM_WINDOWS
#	include <Windows.h>
#endif

namespace Shadow {

void internalGoDownInFlames(const std::string& message, const std::string& file, int line, const std::string& func) {

	std::string output =
		"Shadow Engine bailed out!\nWith this following message:\n" + message
		+ "\n\nAdditional information:\nCXX: "
		+ file + " @ " + std::to_string(line)
		+ "\nMETHOD: " + func
		+ "\nCOMPILED: " + __DATE__ + " at " + __TIME__
		+ "\nENGINE HASH: " + SHADOW_COMMIT_HASH
		+ "\nENGINE VERS: " + SHADOW_VERSION_STRING
		+ "\n\nScreenshot this information and SEND IT TO VINCE!";

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD

	if (!std::filesystem::exists("/usr/bin/zenity")) {
		// Zenity doesn't exist on this system, it's hard to show errors :/
		ERROUT("SHADOW ENGINE BAILOUT");
		ERROUT("%s", output.c_str());
		abort();
	}

	std::system(("/usr/bin/zenity --error --text \"" + output + "\"").c_str());

#elif BX_PLATFORM_WINDOWS

	MessageBoxA(nullptr, output.c_str(), "Shadow Engine", MB_ICONERROR | MB_OK);

#else

	ERROUT("SHADOW ENGINE BAILOUT");
	ERROUT("%s", output.c_str());

#endif

	abort();
}

}