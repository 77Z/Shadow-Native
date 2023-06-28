#ifndef SHADOW_NATIVE_UI_SHADOW_FLINGER_H
#define SHADOW_NATIVE_UI_SHADOW_FLINGER_H

#include "bgfx/bgfx.h"
#include "types.h"

namespace Shadow {
namespace UI {

	struct UIVertex {
		float x;
		float y;
		float z;
	};

	const UIVertex rectangle[] = {
		{ 0.0f, 0.0f, 0.0f },
		{ 4.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 4.0f, 1.0f, 0.0f },
	};

	const u16 rectangleTriList[] = {
		0,
		1,
		2,
		1,
		2,
		3,
	};

	class ShadowFlingerViewport {
		ShadowFlingerViewport(bgfx::ViewId viewId)
			: viewId(viewId) { }
		~ShadowFlingerViewport() { }

	private:
		bgfx::ViewId viewId;
	};

}
}

#endif /* SHADOW_NATIVE_UI_SHADOW_FLINGER_H */
