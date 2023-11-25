#include "TextureUtilities.hpp"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bx/bx.h"
#include "snappy.h"
#include <cstdint>

#include "missingtex.snap.ktx.h"
// #include "missingtex.ktx.h"
// #include "missingtex.exr.h"


namespace Shadow {
		
	bgfx::TextureHandle generateMissingTexture(uint32_t width, uint32_t height) {
#if 0
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
#endif

#if 0
		BX_UNUSED(width);
		BX_UNUSED(height);

		const bgfx::Memory* mem = bgfx::alloc(270000);
		unsigned char* texbuf = (unsigned char*)mem->data;

		bx::memSet(texbuf, 0xff, mem->size);

		return bgfx::createTexture2D(
			300, 300, false, 1, bgfx::TextureFormat::RGB8, BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, mem);

#endif


		BX_UNUSED(width);
		BX_UNUSED(height);
		
		std::string out;
		snappy::Uncompress((char*)missing_texture_snap, sizeof(missing_texture_snap), &out);
		const bgfx::Memory* mem = bgfx::makeRef(out.data(), out.size());

		// const bgfx::Memory* mem = bgfx::makeRef(missing_texture_exr, sizeof(missing_texture_exr));

		return bgfx::createTexture2D(
			800,
			800,
			false,
			1,
			bgfx::TextureFormat::RGBA8,
			// bgfx::TextureFormat::RGBA16F,
			BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
			mem
		);
	}

}