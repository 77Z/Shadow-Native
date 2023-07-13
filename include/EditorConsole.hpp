#ifndef SHADOW_NATIVE_EDITOR_CONSOLE_HPP
#define SHADOW_NATIVE_EDITOR_CONSOLE_HPP

#include <string>
#include <vector>
namespace Shadow {
class EditorConsole {
public:
	EditorConsole();
	~EditorConsole();

	void clearLog();
	void addLog(const char* fmt, ...);
	void draw(std::string title, bool* p_open);

	bool autoScroll = true;
	std::vector<char*> items;
	std::vector<const char*> commands;
	std::vector<char*> history;
	int historyPos = -1;
	char inputBuf[256];
};

}

#endif /* SHADOW_NATIVE_EDITOR_CONSOLE_HPP */