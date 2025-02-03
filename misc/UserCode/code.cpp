#include "include/code.hpp"
// #include "ShadowAPI.hpp"
#include <vector>

SHADOW_CODE

// void (*callback)() = NULL;
// extern "C" void register_function(void (*callmeplz)()) { callback = callmeplz; }

extern "C" void register_accessible_methods(std::vector<typename Tp>)

void Start() {
	simplePrint(
		"[PLUGIN] Shadow Engine in debug mode: " + isShadowEngineDebugMode() ? "true" : "false");
	// callback();
}

void Update() { }

void Shutdown() { }