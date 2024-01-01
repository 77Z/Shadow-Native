#ifndef SHADOW_NATIVE_EDITOR_CONTENT_BROWSER
#define SHADOW_NATIVE_EDITOR_CONTENT_BROWSER

#include "Util.hpp"
#include "imgui.h"
#include <bgfx/bgfx.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace Shadow {

class ContentBrowser {
public:
	ContentBrowser();
	~ContentBrowser();

	enum fileTypes_ {
		fileTypes_Folder,
		fileTypes_CppSource,
		fileTypes_HppSource,
		fileTypes_CSource,
		fileTypes_ShadowEngineScene,
		fileTypes_KTXTexture,
		fileTypes_PNGTexture,
		fileTypes_Prefab,
		fileTypes_Other,
	};

	struct fileEntry {
		std::string name;
		std::string fullpath;
		bool isFile;
		bool isFolder;
		fileTypes_ fileType;
		std::string fileTypePrettyName;
		ImU32 fileTypeColor;
	};

	void onUpdate();
	void unload();
	void loadDir(std::vector<fileEntry>* indexToWrite, std::string dir = "/");
	std::string getCurrentDir();
	void reloadCurrentDir();

private:
	std::unordered_map<const char*, bgfx::TextureHandle> fileTypeMap
		= { { "cpp", loadTexture("./Resources/icons/fileIcon.png") },
			  { "hpp", loadTexture("./Resources/icons/hppIcon.png") },
			  { "folder", loadTexture("./Resources/icons/folderIcon.png") },
			  { "scene", loadTexture("./Resources/icons/sceneIcon.png") },
			  { "ktx", loadTexture("./Resources/icons/KTX Texture.png") },
			  { "png", loadTexture("./Resources/icons/PNG Texture.png") },
			  { "prefab", loadTexture("./Resources/icons/prefab.png") },
			  { "other", loadTexture("./Resources/icons/otherIcon.png")} };


	std::vector<fileEntry> activeFileIndex;

	void drawPathNavigationRail();
};

}

#endif /* SHADOW_NATIVE_EDITOR_CONTENT_BROWSER */