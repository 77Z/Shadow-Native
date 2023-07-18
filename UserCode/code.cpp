#include "include/code.hpp"
#include <iostream>

SHADOW_CODE

// void (*callback)() = NULL;
// extern "C" void register_function(void (*callmeplz)()) { callback = callmeplz; }

void Start() {
	std::cout << "WE LOVE WYATT!" << std::endl;
	// callback();
}

void Update() { }

void Shutdown() { }