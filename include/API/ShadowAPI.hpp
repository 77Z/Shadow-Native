#ifndef SHADOW_NATIVE_API_SHADOW_API_HPP
#define SHADOW_NATIVE_API_SHADOW_API_HPP

#define SHADOW_ENGINE_API_VERSION "0.1.0"

#include <string>

struct PluginInfo {
	std::string name;
	std::string description;
	std::string author;
	std::string version;
	std::string iconName;
	std::string license;
};

#define APICALL extern "C"
#define EXPORT __attribute__((visibility("default")))


EXPORT APICALL bool isShadowEngineDebugMode();
EXPORT APICALL void simplePrint(std::string input);

#endif /* SHADOW_NATIVE_API_SHADOW_API_HPP */
