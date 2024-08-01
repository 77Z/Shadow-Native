#ifndef SHADOW_NATIVE_AXE_AXE_CLIP_BROWSER
#define SHADOW_NATIVE_AXE_AXE_CLIP_BROWSER

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
	ClipBrowser(ma_engine* audioEngine);
	~ClipBrowser();

	void onUpdate(bool& p_open);

	void shutdown();

private:
	std::vector<ClipBrowserItemInfo> clips;
	ma_engine* audioEngine;

	void refreshFiles();
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_CLIP_BROWSER */