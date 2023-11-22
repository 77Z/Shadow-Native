#ifndef SHADOW_NATIVE_DEBUG_EDITOR_CONSOLE_HPP
#define SHADOW_NATIVE_DEBUG_EDITOR_CONSOLE_HPP

#include "imgui.h"
#include "imgui/imgui_memory_editor.h"
#include <functional>
#include <string>
#include <vector>

#define SE_EMBED_CONSOLE_CREATE(name)
#define SE_EMBED_CONSOLE_PRINT()

namespace Shadow {

class EditorConsole {
public:
	EditorConsole();
	~EditorConsole();

	struct message {
		message() = default;
		message(const message&) = default;
		message(const char* category, const char* data, void* bindata)
			: category(category)
			, data(data)
			, bindata(bindata) { }

		const char* category;
		const char* data;
		void* bindata;
	};

	void clearLog();
	void addLog(const char* category, const char* fmt, ...);
	void addLogWithBinData(const char* category, void* binData, const char* fmt, ...);
	void onUpdate(bool* p_open);

	MemoryEditor edit;

	void* data;

	bool autoScroll = true;
	std::string currentCategory;
	std::vector<const char*> categories;
	std::vector<message> items;
	std::vector<const char*> commands;
	std::vector<char*> history;
	int historyPos = -1;
	char inputBuf[256];
};

}

#endif /* SHADOW_NATIVE_DEBUG_EDITOR_CONSOLE_HPP */