#ifndef SHADOW_NATIVE_EDITOR_CONTENT_BROWSER
#define SHADOW_NATIVE_EDITOR_CONTENT_BROWSER

#include "Util.hpp"
#include <bgfx/bgfx.h>
#include <string>
#include <unordered_map>
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
	std::unordered_map<const char*, bgfx::TextureHandle> fileTypeMap
		= { { "cpp", loadTexture("./Resources/fileIcon.png") },
			  { "hpp", loadTexture("./Resources/hppIcon.png") },
			  { "folder", loadTexture("./Resources/folderIcon.png") },
			  { "scene", loadTexture("./Resources/sceneIcon.png") } };

	// bgfx::TextureHandle cppIcon;
	// bgfx::TextureHandle hppIcon;
	// bgfx::TextureHandle folderIcon;
	// bgfx::TextureHandle sceneIcon;

	std::vector<fileEntry> activeFileIndex;

	void drawPathNavigationRail();
};

}

#endif /* SHADOW_NATIVE_EDITOR_CONTENT_BROWSER */