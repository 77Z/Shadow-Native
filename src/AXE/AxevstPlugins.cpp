//
// Created by vince on 9/12/25.
//

#include "AxevstPlugins.hpp"

#include "Debug/EditorConsole.hpp"
#include "bx/bx.h"
#include "imgui.h"
#include "json_impl.hpp"

#include <filesystem>

#define EC_THIS "AXE VST"

namespace Shadow::AXE {

AXEVSTPlugins::AXEVSTPlugins() {
	EC_NEWCAT(EC_THIS);

	reloadVSTPlugins();
}

AXEVSTPlugins::~AXEVSTPlugins() = default;

void AXEVSTPlugins::onUpdate(bool& p_open) {
	using namespace ImGui;

	if (!p_open) return;

	if (!Begin("VST Plugins", &p_open)) {
		End();
		return;
	}

	SeparatorText("Available Plugins:");
	SameLine();
	if (SmallButton("re-scan")) reloadVSTPlugins();

	for (auto& vst : indexedVsts) {
		Selectable(vst.prettyName.c_str());
		SetItemTooltip("%s - %s", vst.version.c_str(), vst.vendor.c_str());
	}

	End();
}

std::filesystem::path AXEVSTPlugins::getVSTStorageLocation() {
	std::string path;

#ifdef BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	path = static_cast<std::string>(getenv("HOME")) + "/.vst3/";
#elif BX_PLATFORM_WINDOWS
	path = "C:\\Program Files\\Common Files\\VST3";
#else
#	error "COMPILATION ERROR: AXE DOESN'T KNOW WHERE VSTs GO ON THIS PLATFORM"
#endif

	return std::filesystem::path(path);
}

void AXEVSTPlugins::reloadVSTPlugins() {
	// clear existing index cache before re-filling
	indexedVsts.clear();

	EC_PRINT(EC_THIS, "Discovering plugins:");
	for (auto& vstDir : std::filesystem::directory_iterator(getVSTStorageLocation())) {
		BasicVstEntry entry;
		EC_PRINT(EC_THIS, "- %s", vstDir.path().filename().c_str());

		// Validate that the shared lib file exists (probably should have a more extensive check.
		// e.g. loading it, grabbing plugin name)
		// std::filesystem::exists(vstDir.path() / "Contents" / "");

		entry.name = vstDir.path().filename().c_str();
		entry.path = vstDir.path();


		// Is there a moduleinfo.json we can read?
#if 0
		std::filesystem::path moduleInfoPath = vstDir.path() / "Contents" / "Resources" / "moduleinfo.json";
		if (std::filesystem::exists(moduleInfoPath)) {
			try {
				auto moduleInfoData = JSON::readJsonFile(moduleInfoPath);

				entry.prettyName = moduleInfoData["Name"];
				entry.version = moduleInfoData["Version"];
				entry.vendor = moduleInfoData["Factory Info"]["Vendor"];
				entry.url = moduleInfoData["Factory Info"]["URL"];
				entry.email = moduleInfoData["Factory Info"]["E-Mail"];
			} catch ([[maybe_unused]] const std::exception& e) {
				entry.prettyName = entry.name;
			}
		} else {
			entry.prettyName = entry.name;
		}
#endif
		indexedVsts.emplace_back(entry);
	}
	EC_PRINT(EC_THIS, "-----------------------------------");
}



}