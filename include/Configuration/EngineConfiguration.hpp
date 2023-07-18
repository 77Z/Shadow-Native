#ifndef SHADOW_NATIVE_CONFIGURATION_ENGINE_CONFIGURATION_HPP
#define SHADOW_NATIVE_CONFIGURATION_ENGINE_CONFIGURATION_HPP

#include "Debug/Logger.h"
#include "ShadowWindow.h"
#include "bx/platform.h"
#include "generated/autoconf.h"
#include <cstddef>
#include <cstdlib>
#include <string>

namespace Shadow {
namespace EngineConfiguration {

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	// Safe for usage within filesystem operations
	inline std::string getConfigDir() {
		std::string configDir = CONFIG_EDITOR_CONFIG_DIR_UNIX;
		size_t pos = configDir.find("~/");
		if (pos == std::string::npos) {
			return configDir;
		}
		return configDir.replace(pos, 1, getenv("HOME"));
	}

#elif BX_PLATFORM_WINDOWS
	inline std::string getConfigDir() {
		std::string configDir = CONFIG_EDITOR_CONFIG_DIR_WINDOWS;
		size_t pos = configDir.find("%AppData%");
		if (pos == std::string::npos) {
			return configDir;
		}
#error Not Implemented!
		ERROUT("Not implemented! [%i]", __LINE__);

		return configDir.replace(pos, 9, "C:\Users\Somethings???");
	}

#endif

	const std::string engineConfigFileLoc = getConfigDir() + "/engineConfig.sec";

	struct EngineConfigFile {
		std::string lastOpenProject;
		ShadowWindow::WindowDimensions projectBrowserWindowBounds;
		ShadowWindow::WindowDimensions editorWindowBounds;
	};

	EngineConfigFile getConfig();

	// Creates all the default configs and directories ONLY if they do not
	// already exist.
	// Returns 0 on success, 1 on failure
	int initializeEngineConfig();

}
}

#endif /* SHADOW_NATIVE_CONFIGURATION_ENGINE_CONFIGURATION_HPP */