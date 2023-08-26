// API meant to be called by UserCode and NOT internal code

#include "API/ShadowAPI.hpp"
#include "Debug/Logger.hpp"
#include <string>

bool isShadowEngineDebugMode() {
#ifdef SHADOW_DEBUG_BUILD
	return true;
#else
	return false;
#endif
}

void simplePrint(std::string input) { PRINT("%s", input.c_str()); }