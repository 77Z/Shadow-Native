#ifndef SHADOW_NATIVE_EDITOR_CONTENT_BROWSER
#define SHADOW_NATIVE_EDITOR_CONTENT_BROWSER

#include <bgfx/bgfx.h>

namespace Shadow {

class ContentBrowser {
public:
	ContentBrowser();
	~ContentBrowser();

	void onUpdate();
	void unload();

private:
	bgfx::TextureHandle cppIcon;
	bgfx::TextureHandle hppIcon;
};

}

#endif /* SHADOW_NATIVE_EDITOR_CONTENT_BROWSER */