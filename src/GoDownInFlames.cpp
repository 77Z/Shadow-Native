#include "GoDownInFlames.hpp"
#include <cstdlib>
#include <string>
#include "bx/bx.h"

namespace Shadow {

void internalGoDownInFlames(const std::string& message, const std::string& file, int line) {
#if BX_PLATFORM_LINUX

	std::system(("notify-send -u critical -a \"Shadow Engine\" \"Shadow Engine bailed out!\" \"" + message + "\n\nAdditional information:\nCXX: " + file + "\nL: " + std::to_string(line) + "\"").c_str());

#else

#endif

	abort();
}

}