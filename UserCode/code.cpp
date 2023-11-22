#include "include/code.hpp"
#include "ShadowAPI.hpp"

SHADOW_CODE

// void (*callback)() = NULL;
// extern "C" void register_function(void (*callmeplz)()) { callback = callmeplz; }

void Start() {
	simplePrint(
		"[PLUGIN] Shadow Engine in debug mode: " + isShadowEngineDebugMode() ? "true" : "false");
	// callback();
}

void Update() { }

void Shutdown() { }