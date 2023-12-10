#include "Scene/FlyCamera.hpp"
#include "Mouse.hpp"
#include <cstdint>

namespace Shadow::Scene {
	
FlyCamera::FlyCamera() {
	reset();
	MouseState mouseState;
	update(0.0f, mouseState, true);
}

void FlyCamera::reset() {
	mouseNow.mx = 0;
	mouseNow.my = 0;
	mouseNow.mz = 0;

	mouseLast.mx = 0;
	mouseLast.my = 0;
	mouseLast.mz = 0;

	eye.x = 0.0f;
	eye.y = 0.0f;
	eye.z = 0.0f;
	at.x = 0.0f;
	at.y = 0.0f;
	at.z = 0.0f;
	up.x = 0.0f;
	up.y = 0.0f;
	up.z = 0.0f;

	horizontalAngle = 0.01f;
	verticalAngle = 0.0f;
	mouseSpeed = 0.0020f;
	gamepadSpeed = 0.04f;
	mouseSpeed = 30.0f;
	mouseDown = false;
}


void FlyCamera::update(float deltaTime, const MouseState& mouseState, bool reset) {
	if (reset) {
		mouseLast.mx = mouseState.mx;
		mouseLast.my = mouseState.my;
		mouseLast.mz = mouseState.mz;
		mouseNow = mouseLast;
		mouseDown = false;

		return;
	}

	if (!mouseDown) {
		mouseLast.mx = mouseState.mx;
		mouseLast.my = mouseState.my;
	}

	mouseDown = !!mouseState.buttons[MouseButton::Right];

	if (mouseDown) {
		mouseNow.mx = mouseState.mx;
		mouseNow.my = mouseState.my;
	}

	mouseLast.mz = mouseNow.mz;
	mouseNow.mz = mouseState.mz;

	const float deltaZ = float(mouseNow.mz - mouseLast.mz);

	if (mouseDown) {
		const int32_t deltaX = mouseNow.mx - mouseLast.mx;
		const int32_t deltaY = mouseNow.my - mouseLast.my;

		horizontalAngle += mouseSpeed * float(deltaX);
		verticalAngle -= mouseSpeed * float(deltaY);

		mouseLast.mx = mouseNow.mx;
		mouseLast.my = mouseNow.my;
	}
}

}