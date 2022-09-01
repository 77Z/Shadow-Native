// API meant to be called by UserCode and NOT internal code

#include "API/ShadowAPI.h"

bool isShadowEngineDebugMode() {
#ifdef SHADOW_DEBUG_BUILD
	return true;
#else
	return false;
#endif
}
