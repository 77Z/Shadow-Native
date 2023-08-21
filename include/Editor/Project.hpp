#ifndef SHADOW_NATIVE_EDITOR_PROJECT_HPP
#define SHADOW_NATIVE_EDITOR_PROJECT_HPP

#include <bgfx/bgfx.h>
#include <string>

namespace Shadow::Editor {

struct ProjectEntry {
	std::string name;
	std::string path;
	bgfx::TextureHandle icon;

	ProjectEntry() = default;

	ProjectEntry(std::string name, std::string path, bgfx::TextureHandle icon)
		: name(name)
		, path(path)
		, icon(icon) { }

	ProjectEntry(std::string name, std::string path)
		: name(name)
		, path(path) { }
};

// Can be empty if no project is open, CHECK FOR IT!
std::string getCurrentProjectName();

// Can be empty if no project is open, CHECK FOR IT!
std::string getCurrentProjectPath();

void setCurrentProjectName(std::string name);
void setCurrentProjectPath(std::string path);

}

#endif /* SHADOW_NATIVE_EDITOR_PROJECT_HPP */