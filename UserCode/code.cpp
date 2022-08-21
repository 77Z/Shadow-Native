#include "code.hpp"
#include <iostream>
#include "Runtime.h"

SHADOW_CODE

void Start() {
	std::cout << "Hello from USER LAND" << std::endl;
//	Shadow::ShutdownRuntime();
}

void Update() {}
