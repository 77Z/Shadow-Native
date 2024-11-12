#ifndef SHADOW_NATIVE_DEBUG_EDITOR_CONSOLE_HPP
#define SHADOW_NATIVE_DEBUG_EDITOR_CONSOLE_HPP

#define EC_NEWCAT(category) Shadow::EditorConsole::newCategory(category);

#define EC_PRINT(category, fmt, ...) Shadow::EditorConsole::print(0, __FILE_NAME__, category, fmt, ##__VA_ARGS__)
#define EC_WARN(category, fmt, ...) Shadow::EditorConsole::print(1, __FILE_NAME__, category, fmt, ##__VA_ARGS__)
#define EC_ERROUT(category, fmt, ...) Shadow::EditorConsole::print(2, __FILE_NAME__, category, fmt, ##__VA_ARGS__)

namespace Shadow::EditorConsole {
	// Category names can't be any larger than 500 or so characters
	void newCategory(const char* category);
	void print(int severity, const char* caller, const char* category, const char* fmt, ...);
	void clearLog();

	namespace Frontend {
		void onUpdate();
	}
}

#endif /* SHADOW_NATIVE_DEBUG_EDITOR_CONSOLE_HPP */