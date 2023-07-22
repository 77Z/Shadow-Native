#ifndef SHADOW_NATIVE_EDITOR_PROJECT_BROWSER_HPP
#define SHADOW_NATIVE_EDITOR_PROJECT_BROWSER_HPP

#include "bgfx/bgfx.h"
#include <string>

namespace Shadow::Editor {

struct ProjectEntry {
	std::string name;
	std::string path;
	bgfx::TextureHandle icon;
};

int startProjectBrowser();

}

#endif /* SHADOW_NATIVE_EDITOR_PROJECT_BROWSER_HPP */