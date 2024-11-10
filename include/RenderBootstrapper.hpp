#ifndef SHADOW_NATIVE_RENDER_BOOTSTRAPPER_HPP
#define SHADOW_NATIVE_RENDER_BOOTSTRAPPER_HPP

#include "ShadowWindow.hpp"
#include <bgfx/bgfx.h>

namespace Shadow {

class RenderBootstrapper {
public:
	RenderBootstrapper(ShadowWindow* window, bgfx::RendererType::Enum renderer, bgfx::ViewId viewId,
		bool vsync = true, const char* imguiIni = "imgui.ini");

	void startFrame();
	void endFrame();

	void shutdown();

private:
	ShadowWindow* window;
	bool vsync;
};

}

#endif /* SHADOW_NATIVE_RENDER_BOOTSTRAPPER_HPP */