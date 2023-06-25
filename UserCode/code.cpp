#include "include/code.hpp"
#include "../include/db.h"
#include "Runtime.h"
#include <iostream>

SHADOW_CODE

void (*callback)() = NULL;
extern "C" void register_function(void (*callmeplz)()) { callback = callmeplz; }

void Start() {
	std::cout << "Hello from USER LAND" << std::endl;
	Shadow::Database newDB("./datatatata");
	newDB.write("HELLO", "HELLO");
	callback();
}

void Update() { }

void Shutdown() { }