#ifndef SHADOW_NATIVE_API_SHADOW_API_HPP
#define SHADOW_NATIVE_API_SHADOW_API_HPP

#include <string>

extern "C" {

bool isShadowEngineDebugMode();
void simplePrint(std::string input);
}

#endif /* SHADOW_NATIVE_API_SHADOW_API_HPP */
