#ifndef SHADOW_NATIVE_SCENE_COMPONENTS_HPP
#define SHADOW_NATIVE_SCENE_COMPONENTS_HPP

#include "Mesh.hpp"
#include "bx/math.h"
#include "uuid_impl.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <glm/glm.hpp>
#include <string>

namespace Shadow {

struct IDComponent {
	uuids::uuid ID;

	IDComponent() = default;
	IDComponent(const IDComponent&) = default;
	IDComponent(const uuids::uuid& id)
		: ID(id) { }
};

struct TagComponent {
	std::string tag;

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const std::string& tag)
		: tag(tag) { }
};

struct TransformComponent {
	// glm::mat4 transform = glm::mat4(1.0f);
	bx::Vec3 translation = bx::Vec3(0.0f, 0.0f, 0.0f);
	bx::Vec3 rotation = bx::Vec3(0.0f, 0.0f, 0.0f);
	bx::Vec3 scale = bx::Vec3(1.0f, 1.0f, 1.0f);

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;

	// operator glm::mat4&() { return transform; }
	// operator const glm::mat4&() const { return transform; }
};

struct MeshComponent {

	Mesh mesh();

	MeshComponent() = default;
	MeshComponent(const MeshComponent&) = default;
};

struct ShapePusherComponent {
	uint32_t abgr = 0xff0000ff;

	ShapePusherComponent() = default;
	ShapePusherComponent(const ShapePusherComponent&) = default;
	ShapePusherComponent(const uint32_t& color)
		: abgr(color) { }
};

struct CubeComponent {
	float offset = 0.0f;

	CubeComponent() = default;
	CubeComponent(float offset)
		: offset(offset) { }
};

}

#endif /* SHADOW_NATIVE_SCENE_COMPONENTS_HPP */