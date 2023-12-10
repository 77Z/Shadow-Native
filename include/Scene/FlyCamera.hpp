#ifndef SHADOW_NATIVE_SCENE_FLY_CAMERA_HPP
#define SHADOW_NATIVE_SCENE_FLY_CAMERA_HPP

#include "Mouse.hpp"
#include "bx/bx.h"
#include "bx/math.h"
#include <cstdint>

namespace Shadow::Scene {
	
class FlyCamera {
public:
	FlyCamera();
	~FlyCamera();

	void reset();

	void update(float deltaTime, const MouseState& mouseState, bool reset);


	struct MouseCoords {
		int32_t mx;
		int32_t my;
		int32_t mz;
	};

	MouseCoords mouseNow;
	MouseCoords mouseLast;

	bx::Vec3 eye = bx::InitZero;
	bx::Vec3 at = bx::InitZero;
	bx::Vec3 up = bx::InitZero;
	float horizontalAngle;
	float verticalAngle;

	float mouseSpeed;
	float gamepadSpeed;
	float moveSpeed;
	
	bool mouseDown;
};

}

#endif /* SHADOW_NATIVE_SCENE_FLY_CAMERA_HPP */