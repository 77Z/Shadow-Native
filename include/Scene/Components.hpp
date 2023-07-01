#ifndef SHADOW_NATIVE_SCENE_COMPONENTS_HPP
#define SHADOW_NATIVE_SCENE_COMPONENTS_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace Shadow {

/* struct TransformComponent {
	glm::mat4 transform = glm::mat4(1.0f);

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const glm::mat4& transform)
		: transform(transform) { }

	operator glm::mat4&() { return transform; }
	operator const glm::mat4&() const { return transform; }
}; */

/* struct TransformComponent {
	float transform[16];

	TransformComponent() = default;
	TransformComponent(float mtx[16]) { memcpy(transform, mtx, size_t(transform)); }
};

struct MeshComponent { };

struct ShapePusherComponent {
	uint32_t abgr = 0xff0000ff;

	ShapePusherComponent() = default;
	ShapePusherComponent(const ShapePusherComponent&) = default;
	ShapePusherComponent(const uint32_t& color)
		: abgr(color) { }
}; */

struct CubeComponent {
	float offset = 0.0f;

	CubeComponent() = default;
	CubeComponent(float offset)
		: offset(offset) { }
};

}

#endif /* SHADOW_NATIVE_SCENE_COMPONENTS_HPP */