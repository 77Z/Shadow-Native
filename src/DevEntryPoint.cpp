#include "Debug/Logger.hpp"
#include "mono/monoRuntime.hpp"

namespace Shadow {

static void pad() { PRINT("\n\n\n\n"); }

int devEntry() {

	WARN("MONO RUNTIME STUFF BRUZ");
	pad();

	ShadowMonoRuntime shadowMonoRuntime;

	return 0;
}

}