#ifndef SHADOW_NATIVE_TEXTURE_UTILITIES
#define SHADOW_NATIVE_TEXTURE_UTILITIES

#include "bgfx/bgfx.h"
#include <cstdint>

namespace Shadow {

	//!-----------------------------------------//
	// TODO: Move texture loading methods here //
	//!---------------------------------------//

	bgfx::TextureHandle generateMissingTexture(uint32_t width, uint32_t height);

}

#endif /* SHADOW_NATIVE_TEXTURE_UTILITIES */