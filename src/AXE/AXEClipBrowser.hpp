#ifndef SHADOW_NATIVE_AXE_AXE_CLIP_BROWSER
#define SHADOW_NATIVE_AXE_AXE_CLIP_BROWSER

#include <filesystem>
#include <string>
#include <vector>
#include "AXETypes.hpp"
#include "miniaudio.h"

namespace Shadow::AXE {

namespace fs = std::filesystem;

struct ClipBrowserItemInfo {
	// Path to clip when it is stored in the AXE global library
	fs::path clipPath;
	AudioFileTypes_ audioFileType;
	
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
	void addFileToLibrary(const std::filesystem::path filepath);

	/// Same as `addFileToLibrary` but accepts vectors  of filepaths
	// void addFilesToLibrary(const std::vector<std::string> filepaths);

private:
	std::filesystem::path globalLibraryPath;
	std::vector<ClipBrowserItemInfo> clips;
	ma_result aeResult;
	ma_engine_config clipBrowserAudioEngineConfig;
	ma_engine clipBrowserAudioEngine;

	void refreshFiles();
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_CLIP_BROWSER */