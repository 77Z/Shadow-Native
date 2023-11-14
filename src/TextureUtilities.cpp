#include "TextureUtilities.hpp"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bx/bx.h"
#include <cstdint>

namespace Shadow {
		
	bgfx::TextureHandle generateMissingTexture(uint32_t width, uint32_t height) {
		const bgfx::Memory* mem = bgfx::alloc(width * height * sizeof(uint32_t));

		uint32_t* pixels = (uint32_t*)mem->data;
		bx::memSet(pixels, 0, mem->size);

		const uint32_t black = 0xFF000000;
		const uint32_t purple = 0xFF9900ff;

		for (uint32_t x = 0; x < width; x++) {
			for (uint32_t y = 0; y < height; y++) {
				if (x % 10 == 0 || y % 10 == 0)
					pixels[x] = black;
				else
					pixels[x] = purple;
			}
		}

		return bgfx::createTexture2D(uint16_t(width), uint16_t(height), false, 1, bgfx::TextureFormat::BGRA8, BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP, mem);
	}

}