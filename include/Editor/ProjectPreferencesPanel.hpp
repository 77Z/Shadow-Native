#ifndef SHADOW_NATIVE_EDITOR_PROJECT_PREFERENCES_PANEL_HPP
#define SHADOW_NATIVE_EDITOR_PROJECT_PREFERENCES_PANEL_HPP

#include <bgfx/bgfx.h>

namespace Shadow::Editor {

class ProjectPreferencesPanel {
public:
	ProjectPreferencesPanel();
	~ProjectPreferencesPanel();

	void onUpdate();
	void unload();
	void open();

private:
	bgfx::TextureHandle cppIcon;
	bgfx::TextureHandle hppIcon;
};

}

#endif /* SHADOW_NATIVE_EDITOR_PROJECT_PREFERENCES_PANEL_HPP */