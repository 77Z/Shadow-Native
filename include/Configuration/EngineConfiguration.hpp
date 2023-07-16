#ifndef SHADOW_NATIVE_CONFIGURATION_ENGINE_CONFIGURATION_HPP
#define SHADOW_NATIVE_CONFIGURATION_ENGINE_CONFIGURATION_HPP

#include "ShadowWindow.h"
#include "bx/platform.h"
#include "generated/autoconf.h"
#include <string>

namespace Shadow {
namespace EngineConfiguration {

#if BX_PLATFORM_LINUX
	const std::string configDir = CONFIG_EDITOR_CONFIG_DIR_UNIX;
#elif BX_PLATFORM_WINDOWS
	const std::string ConfigDir = CONFIG_EDITOR_CONFIG_DIR_WINDOWS;
#endif

	const std::string engineConfigFileLoc = configDir + "/engineConfig.sec";

	struct EngineConfigFile {
		std::string lastOpenProject;
		ShadowWindow::WindowDimensions projectBrowserWindowBounds;
		ShadowWindow::WindowDimensions editorWindowBounds;
	};

	EngineConfigFile getConfig();

}
}

#endif /* SHADOW_NATIVE_CONFIGURATION_ENGINE_CONFIGURATION_HPP */