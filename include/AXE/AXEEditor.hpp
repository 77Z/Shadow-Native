#ifndef SHADOW_NATIVE_AXE_AXE_EDITOR_HPP
#define SHADOW_NATIVE_AXE_AXE_EDITOR_HPP

#include "AXE/AXEProject.hpp"
#include <vector>

namespace Shadow::AXE {

struct AXETrack { };

struct InternalAXEProject {
	std::string name;
	std::string path;
	std::vector<AXETrack> tracks;

	InternalAXEProject() = default;

	InternalAXEProject(const std::string name, const std::string path)
		: name(name)
		, path(path) { }
};

int startAXEEditor(AXEProjectEntry project);

}

#endif /* SHADOW_NATIVE_AXE_AXE_EDITOR_HPP */