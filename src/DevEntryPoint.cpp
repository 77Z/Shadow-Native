#include "DevEntryPoint.hpp"
#include "Core.hpp"
#include "Debug/Logger.h"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneSerializer.hpp"
#include "bgfx/bgfx.h"
#include "uuid.h"
#include "uuid_impl.hpp"

namespace Shadow {

static void pad() { PRINT("\n\n\n\n"); }

int devEntry() {

	bgfx::Init init;
	init.type = bgfx::RendererType::Noop;
	bgfx::init(init);

	pad();

	WARN("Development entry point");

	// PRINT("Here's a UUID: %s", uuids::to_string(generateUUID()).c_str());

	Reference<Scene> scene = CreateReference<Scene>("My Scene");
	SceneSerializer ss(scene);

	Entity bruh = scene->createEntity("bruh");
	bruh.GetComponent<TagComponent>().tag = "overidden";
	bruh.addComponent<TransformComponent>();
	bruh.GetComponent<TransformComponent>().translation.x = 69.420;

	Entity cube = scene->createEntity("My Cube");
	cube.addComponent<CubeComponent>(1.0f);

	ss.serialize("./scene.sescene");

	WARN("SPLIT");

	Reference<Scene> newScene = CreateReference<Scene>("Doesn't Matter");
	SceneSerializer newSs(newScene);

	newSs.deserialize("./scene.sescene");

	pad();
	return 0;
}

}