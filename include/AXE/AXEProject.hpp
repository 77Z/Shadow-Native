#ifndef SHADOW_NATIVE_AXE_AXE_PROJECT_HPP
#define SHADOW_NATIVE_AXE_AXE_PROJECT_HPP

#include <string>

namespace Shadow::AXE {

struct AXEProjectEntry {
	std::string name;
	std::string path;

	AXEProjectEntry() = default;

	AXEProjectEntry(const std::string name, const std::string path)
		: name(name)
		, path(path) { }
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_PROJECT_HPP */