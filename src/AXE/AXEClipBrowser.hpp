#ifndef SHADOW_NATIVE_AXE_AXE_CLIP_BROWSER
#define SHADOW_NATIVE_AXE_AXE_CLIP_BROWSER

#include <filesystem>
#include <string>
#include <vector>
#include "miniaudio.h"

namespace Shadow::AXE {

struct ClipBrowserItemInfo {
	std::string fullpath;
	std::string relativePath;
	std::string prettyName;

	std::string prettyType;

	bool isDirectory;
};

class ClipBrowser {
public:
	ClipBrowser();
	~ClipBrowser();

	void onUpdate(bool& p_open);

	void shutdown();

	/// Copies the file at specified `filepath` to the user's GlobalLibrary
	/// directory and automatically reloads the `clips` vector
	void addFileToLibrary(const std::string& filepath);

	/// Same as `addFileToLibrary` but accepts vectors  of filepaths
	// void addFilesToLibrary(const std::vector<std::string> filepaths);

private:
	std::string globalLibraryPath;
	std::vector<ClipBrowserItemInfo> clips;
	ma_engine* audioEngine;
	// std::filesystem::path globalLibraryPath;

	void refreshFiles();
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_CLIP_BROWSER */