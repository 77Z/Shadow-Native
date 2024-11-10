#include "Core.hpp"
#include "Debug/Logger.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneSerializer.hpp"
#include <bgfx/bgfx.h>
#include <filesystem>
#include <string>

namespace Shadow::Editor {

int startSlimEditor(const std::string& scenePath) {
	if (!std::filesystem::exists(scenePath)) {
		ERROUT("SCENE PROVIDED DOES NOT EXIST: %s", scenePath.c_str());
		return 1;
	}

	PRINT("Starting SlimEditor with scene %s", scenePath.c_str());

	Reference<Scene> scene = CreateReference<Scene>();
	SceneSerializer sceneSerializer(scene);
	sceneSerializer.deserialize(scenePath);

	return 0;
}

}