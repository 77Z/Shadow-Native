#include "GoDownInFlames.hpp"
#include <cstdlib>
#include <string>
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

	//TODO: Replace this with the above output string.
	std::system(("notify-send -u critical -a \"Shadow Engine\" \"Shadow Engine bailed out!\" \"" + message + "\n\nAdditional information:\nCXX: " + file + "\nL: " + std::to_string(line) + "\"").c_str());

#elif BX_PLATFORM_WINDOWS

	MessageBoxA(nullptr, output.c_str(), "Shadow Engine", MB_ICONERROR | MB_OK);

#endif

	abort();
}

}