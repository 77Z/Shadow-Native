#ifndef SHADOW_NATIVE_EDITOR_CONTENT_BROWSER
#define SHADOW_NATIVE_EDITOR_CONTENT_BROWSER

#include <bgfx/bgfx.h>
#include <string>
#include <vector>

namespace Shadow {

class ContentBrowser {
public:
	ContentBrowser();
	~ContentBrowser();

	struct fileTypes {
		enum Enum { Folder, CppSource, HppSource, CSource, ShadowEngineScene };
	};

	struct fileEntry {
		std::string name;
		std::string fullpath;
		bool isFile;
		bool isFolder;
		fileTypes fileType;
	};

	void onUpdate();
	void unload();
	void loadDir(std::vector<fileEntry>* indexToWrite, std::string dir = "/");
	std::string getCurrentDir();

private:
	bgfx::TextureHandle cppIcon;
	bgfx::TextureHandle hppIcon;
	bgfx::TextureHandle folderIcon;

	std::vector<fileEntry> activeFileIndex;

	void drawPathNavigationRail();
};

}

#endif /* SHADOW_NATIVE_EDITOR_CONTENT_BROWSER */