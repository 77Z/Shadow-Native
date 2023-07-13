#ifndef SHADOW_NATIVE_DEBUG_EDITOR_CONSOLE_HPP
#define SHADOW_NATIVE_DEBUG_EDITOR_CONSOLE_HPP

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

	void clearLog();
	void addLog(const char* fmt, ...);
	void onUpdate(bool* p_open);

	bool autoScroll = true;
	std::vector<char*> items;
	std::vector<const char*> commands;
	std::vector<char*> history;
	int historyPos = -1;
	char inputBuf[256];
	std::string consoleName;
};

class EditorConsoleManager {
public:
	EditorConsoleManager();
	~EditorConsoleManager();

	void createNewConsole(std::string name);
	void onUpdate();
	bool showConsoles = true;

private:
	std::vector<std::reference_wrapper<EditorConsole>> consoles;
};

}

#endif /* SHADOW_NATIVE_DEBUG_EDITOR_CONSOLE_HPP */