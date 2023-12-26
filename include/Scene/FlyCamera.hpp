#ifndef SHADOW_NATIVE_SCENE_FLY_CAMERA_HPP
#define SHADOW_NATIVE_SCENE_FLY_CAMERA_HPP

#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "bx/bx.h"
#include "bx/math.h"
#include <cstdint>

namespace Shadow::SceneNamespace {
	
class FlyCamera {
public:
	FlyCamera(Mouse* mouse, Keyboard* keyboard);
	~FlyCamera();

	void reset();

	void update(float deltaTime, bool reset);

	void setPosition(const bx::Vec3& pos);

	void setVerticalAngle(float angle);
	void setHorizontalAngle(float angle);

	void getViewMtx(float* viewMtx);

	struct MouseCoords {
		int32_t mx;
		int32_t my;
		int32_t mz;
	};

	// MouseCoords mouseNow;
	// MouseCoords mouseLast;

	bx::Vec3 eye = bx::InitZero;
	bx::Vec3 at = bx::InitZero;
	bx::Vec3 m_up = bx::InitZero;
	float horizontalAngle;
	float verticalAngle;

	float mouseSpeed;
	float gamepadSpeed;
	float moveSpeed;
	
	bool mouseDown;

private:
	Mouse* mouse;
	Keyboard* keyboard;
};

}

#endif /* SHADOW_NATIVE_SCENE_FLY_CAMERA_HPP */