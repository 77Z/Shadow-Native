#pragma once

// Hack to workaround C++ Mangling symbol names
#define SHADOW_CODE                                                                                \
	extern "C" void passthroughStart() { Start(); }

void Start();
void Update();
void Shutdown();