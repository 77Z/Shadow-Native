#include "Scene/SceneSerializer.hpp"
#include "Core.hpp"
#include "Debug/Logger.hpp"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"
#include "json_impl.hpp"
#include "uuid.h"
#include "uuid_impl.hpp"
#include <cstdint>
#include <fstream>
#include <vector>

namespace Shadow {

SceneSerializer::SceneSerializer(const Reference<Scene>& scene)
	: scene(scene) { }

SceneSerializer::~SceneSerializer() { }

void SceneSerializer::serialize(const std::string filepath) {

	sceneJson["SceneName"] = scene->sceneName;

	sceneJson["Entities"] = json::array();

	scene->m_Registry.each([&](auto entityID) {
		Entity entity = { entityID, scene.get() };
		if (!entity)
			return;

		serializeEntity(entity);
	});

	PRINT("Serializing scene to %s", filepath.c_str());
	PRINT("Data: %s", sceneJson.dump(4).c_str());

	JSON::dumpJsonToBson(sceneJson, filepath);

	/* Snappy Compression : Yeah this is pretty rough
	std::vector<uint8_t> bson = json::to_bson(sceneJson);
	std::ofstream outfile(filepath);
	std::stringstream ss;
	std::string out;
	for (int i = 0; i < bson.size(); i++) {
		ss << bson[i];
	}
	std::string sstostr = ss.str();
	snappy::Compress(sstostr.data(), sstostr.size(), &out);
	outfile << out;
	outfile.close(); */
}

void SceneSerializer::serializeEntity(Entity entity) {
	PPK_ASSERT(entity.hasComponent<IDComponent>());

	json serializedEntity;

	serializedEntity["uuid"] = uuids::to_string(entity.GetComponent<IDComponent>().ID);
	serializedEntity["tag"] = entity.GetComponent<TagComponent>().tag;

	auto componentsArr = json::array();

	if (entity.hasComponent<TransformComponent>()) {
		auto& transformComponent = entity.GetComponent<TransformComponent>();
		json componentObj;
		componentObj["type"] = "TransformComponent";

		componentObj["tx"] = transformComponent.translation.x;
		componentObj["ty"] = transformComponent.translation.y;
		componentObj["tz"] = transformComponent.translation.z;

		componentObj["rx"] = transformComponent.rotation.x;
		componentObj["ry"] = transformComponent.rotation.y;
		componentObj["rz"] = transformComponent.rotation.z;

		componentObj["sx"] = transformComponent.scale.x;
		componentObj["sy"] = transformComponent.scale.y;
		componentObj["sz"] = transformComponent.scale.z;

		componentsArr.push_back(componentObj);
	}

	if (entity.hasComponent<CubeComponent>()) {
		auto& cubeComponent = entity.GetComponent<CubeComponent>();
		json componentObj;
		componentObj["type"] = "CubeComponent";

		componentObj["offset"] = cubeComponent.offset;

		componentsArr.push_back(componentObj);
	}

	serializedEntity["components"] = componentsArr;

	sceneJson["Entities"].push_back(serializedEntity);
}

bool SceneSerializer::deserialize(const std::string filepath) {
	std::ifstream infile(filepath);

	if (!infile)
		return false;

	json deserializedScene = json::from_bson(infile);

	infile.close();

	scene->sceneName = deserializedScene["SceneName"];

	scene->m_Registry.each([this](auto entity) {
		//TODO: more should happen here in the future
		// unloading models etc.
		
		// This is scene.get() because `scene` is a Reference
		scene->destroyEntity({entity, scene.get()});
	});
	
	// scene->destroyEntity(Entity entity)

	for (auto entity : deserializedScene["Entities"]) {
		std::string tag = entity["tag"];
		uuids::uuid uuid = uuids::uuid::from_string((std::string)entity["uuid"]).value();

		Entity deserializedEntity = scene->createEntityWithUUID(uuid, tag);

		PRINT("Entity %s with ID %s :", tag.c_str(), uuids::to_string(uuid).c_str());

		for (auto component : entity["components"]) {
			std::string type = component["type"];
			PRINT("---- Component type %s", type.c_str());

			if (type == "TransformComponent") {
				PRINT("-------- tx %.6f", (float)component["tx"]);
				PRINT("-------- ty %.6f", (float)component["ty"]);
				PRINT("-------- tz %.6f", (float)component["tz"]);
				PRINT("-------- rx %.6f", (float)component["rx"]);
				PRINT("-------- ry %.6f", (float)component["ry"]);
				PRINT("-------- rz %.6f", (float)component["rz"]);
				PRINT("-------- sx %.6f", (float)component["sx"]);
				PRINT("-------- sy %.6f", (float)component["sy"]);
				PRINT("-------- sz %.6f", (float)component["sz"]);

				deserializedEntity.addComponent<TransformComponent>(
					(float)component["tx"], (float)component["ty"], (float)component["tz"],
					(float)component["rx"], (float)component["ry"], (float)component["rz"],
					(float)component["sx"], (float)component["sy"], (float)component["sz"]
				);
				
			} else if (type == "CubeComponent") {
				PRINT("-------- offset %.6f", (float)component["offset"]);

				deserializedEntity.addComponent<CubeComponent>((float)component["offset"]);
			} else if (type == "MeshComponent") {
				PRINT("-------- Location: %s", ((std::string)component["location"]).c_str());

				deserializedEntity.addComponent<MeshComponent>(((std::string)component["location"]).c_str());
			} else if (type == "ShaderComponent") {
				PRINT("-------- Frag shader: %s", ((std::string)component["frag"]).c_str());
				PRINT("-------- Vert shader: %s", ((std::string)component["vert"]).c_str());

				deserializedEntity.addComponent<ShaderComponent>((std::string)component["frag"], (std::string)component["vert"]);
			}
		}
	}

	return true;
}

}