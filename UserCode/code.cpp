#include "code.hpp"
#include <iostream>
#include "Runtime.h"

SHADOW_CODE

/*void (*callback)() = NULL;
extern "C" void register_function(void (*callmeplz)()) {
	callback = callmeplz;
}*/

void Start() {
	std::cout << "Hello from USER LAND" << std::endl;
//	callback();
}

void Update() {}
