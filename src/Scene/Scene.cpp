#include "Scene/Scene.hpp"
#include "Debug/Logger.hpp"
#include <bgfx/bgfx.h>
#include <cstdint>
// #include <glm/fwd.hpp>
// #include <glm/glm.hpp>

#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "bx/math.h"
#include "uuid_impl.hpp"

struct PosColorVertex {
	float x;
	float y;
	float z;
	uint32_t abgr;
};

static PosColorVertex cubeVertices[] = {
	{ -1.0f, 1.0f, 1.0f, 0xff000000 },
	{ 1.0f, 1.0f, 1.0f, 0xff0000ff },
	{ -1.0f, -1.0f, 1.0f, 0xff00ff00 },
	{ 1.0f, -1.0f, 1.0f, 0xff00ffff },
	{ -1.0f, 1.0f, -1.0f, 0xffff0000 },
	{ 1.0f, 1.0f, -1.0f, 0xff0000ff },
	{ -1.0f, -1.0f, -1.0f, 0xffffff00 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeTriList[] = {
	0,
	1,
	2,
	1,
	3,
	2,
	4,
	6,
	5,
	5,
	6,
	7,
	0,
	2,
	4,
	4,
	2,
	6,
	1,
	5,
	3,
	5,
	7,
	3,
	0,
	4,
	1,
	4,
	5,
	1,
	2,
	3,
	6,
	6,
	3,
	7,
};

// TODO: GET RID OF ME
static unsigned int counter = 0;

namespace Shadow {

Scene::Scene(std::string name)
	: sceneName(name) {
	init();
}

Scene::Scene() { init(); }

Scene::~Scene() { }

void Scene::unload() {
	bgfx::destroy(vbh);
	bgfx::destroy(ibh);
}

void Scene::init() {

	pcvDecl.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();
	vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), pcvDecl);
	ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

	/* struct MeshComponent {
		float value;
	};

	struct TransformComponent {
		glm::mat4 transform;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)
			: transform(transform) { }

		operator glm::mat4&() { return transform; }
		operator const glm::mat4&() const { return transform; }
	};

	entt::entity demoEntity = m_Registry.create();
	m_Registry.emplace<TransformComponent>(demoEntity, glm::mat4(1.0f));

	auto& transform = m_Registry.get<TransformComponent>(demoEntity);

	auto view = m_Registry.view<TransformComponent>();
	for (auto entity : view) {
		TransformComponent& tf = view.get<TransformComponent>(entity);
		PRINT("Entity id: %i", (uint32_t)entity);
		PRINT("Transform x%f", tf.transform);
	} */
}

Entity Scene::createEntity(const std::string& name) {
	return createEntityWithUUID(generateUUID(), name);
}

Entity Scene::createEntityWithUUID(uuids::uuid uuid, const std::string& name) {
	Entity entity = { m_Registry.create(), this };

	entity.addComponent<IDComponent>(uuid);
	// TODO: Maybe transforms should come with entities by default in the future
	// entity.addComponent<TransformComponent>();
	auto& tag = entity.addComponent<TagComponent>();
	tag.tag = name.empty() ? "Unnamed Actor" : name;

	return entity;
}

void Scene::destroyEntity(Entity entity) {
	entityMap.erase(entity.getUUID());
	m_Registry.destroy(entity);
}

void Scene::onUpdate(bgfx::ViewId viewid, bgfx::ProgramHandle program) {

	auto view = m_Registry.view<CubeComponent>();
	for (auto entity : view) {
		// auto& transform = group.get<TransformComponent>(entity);
		// auto& shape = group.get<ShapePusherComponent>(entity);

		// auto& offset = m_Registry.get<CubeComponent>(entity);
		auto& transform = m_Registry.get<TransformComponent>(entity);

		float tfMtx[16];
		bx::mtxSRT(tfMtx, transform.scale.x, transform.scale.y, transform.scale.z,
			transform.rotation.x, transform.rotation.y + (counter / 50.0f), transform.rotation.z,
			transform.translation.x, transform.translation.y, transform.translation.z);

		bgfx::setTransform(tfMtx, 1);
		bgfx::setVertexBuffer(0, vbh);
		bgfx::setIndexBuffer(ibh);

		counter++;
		bgfx::submit(viewid, program);
	}
}

}