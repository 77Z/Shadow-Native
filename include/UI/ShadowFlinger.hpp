#ifndef SHADOW_NATIVE_UI_SHADOW_FLINGER_HPP
#define SHADOW_NATIVE_UI_SHADOW_FLINGER_HPP

#include "bgfx/bgfx.h"
#include "types.hpp"

namespace Shadow::UI {

struct UIVertex {
	float x;
	float y;
	float z;
};

class ShadowFlingerViewport {
public:
	ShadowFlingerViewport(bgfx::ViewId viewId);

	void resized();
	void draw(bgfx::ProgramHandle program, int width, int height);
	void unload();

private:
	bgfx::ViewId viewId;
	bgfx::VertexLayout drawDecl;

	bgfx::VertexBufferHandle vbh;
	bgfx::IndexBufferHandle ibh;
};

}

#endif /* SHADOW_NATIVE_UI_SHADOW_FLINGER_HPP */
