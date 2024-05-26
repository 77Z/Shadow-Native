#include "Scene/FlyCamera.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "bx/constants.h"
#include "bx/math.h"
#include <cstdint>

static bool cameraForwardPressed = false;
static bool cameraBackwardPressed = false;
static bool cameraLeftPressed = false;
static bool cameraRightPressed = false;
static bool cameraUpPressed = false;
static bool cameraDownPressed = false;

namespace Shadow::SceneNamespace {

FlyCamera::FlyCamera(Mouse* mouse, Keyboard* keyboard)
	: mouse(mouse)
	, keyboard(keyboard)
{
	reset();
	MouseState mouseState;
	update(0.0f, true);

	if (keyboard == nullptr) return;
	
	keyboard->registerKeyCallback([](KeyButton_ key, bool down, KeyModifiers_ mods) {
		BX_UNUSED(mods);

		if (key == KeyButton_W && down) cameraForwardPressed = true;
		if (key == KeyButton_W && !down) cameraForwardPressed = false;

		if (key == KeyButton_S && down) cameraBackwardPressed = true;
		if (key == KeyButton_S && !down) cameraBackwardPressed = false;

		if (key == KeyButton_A && down) cameraLeftPressed = true;
		if (key == KeyButton_A && !down) cameraLeftPressed = false;

		if (key == KeyButton_D && down) cameraRightPressed = true;
		if (key == KeyButton_D && !down) cameraRightPressed = false;

		if (key == KeyButton_E && down) cameraUpPressed = true;
		if (key == KeyButton_E && !down) cameraUpPressed = false;

		if (key == KeyButton_Q && down) cameraDownPressed = true;
		if (key == KeyButton_Q && !down) cameraDownPressed = false;
	});
}

FlyCamera::~FlyCamera() {}

void FlyCamera::reset() {
	eye.x = 0.0f;
	eye.y = 0.0f;
	eye.z = 0.0f;
	at.x = 0.0f;
	at.y = 0.0f;
	at.z = 0.0f;
	m_up.x = 0.0f;
	m_up.y = 0.0f;
	m_up.z = 0.0f;

	horizontalAngle = 0.01f;
	verticalAngle = 0.0f;
	mouseSpeed = 0.0020f;
	gamepadSpeed = 0.04f;
	moveSpeed = 30.0f;
	mouseDown = false;
}


void FlyCamera::update(float deltaTime, bool reset) {
	mouseDown = mouse->isRightMouseDown();
  
	if (reset && !mouseDown) {
		return;
	}

	if (mouseDown) {
		horizontalAngle += mouseSpeed * float(mouse->getMouseXDiff());
		verticalAngle -= mouseSpeed * float(mouse->getMouseYDiff());
	}

	const bx::Vec3 direction = {
		bx::cos(verticalAngle) * bx::sin(horizontalAngle),
		bx::sin(verticalAngle),
		bx::cos(verticalAngle) * bx::cos(horizontalAngle),
	};

	const bx::Vec3 right = {
		bx::sin(horizontalAngle - bx::kPiHalf),
		0.0f,
		bx::cos(horizontalAngle - bx::kPiHalf)
	};

	const bx::Vec3 up = bx::cross(right, direction);

	if (cameraForwardPressed) eye = bx::mad(direction, deltaTime * moveSpeed, eye);
	if (cameraBackwardPressed) eye = bx::mad(direction, -deltaTime * moveSpeed, eye);
	if (cameraLeftPressed) eye = bx::mad(right, deltaTime * moveSpeed, eye);
	if (cameraRightPressed) eye = bx::mad(right, -deltaTime * moveSpeed, eye);
	if (cameraUpPressed) eye = bx::mad(up, deltaTime * moveSpeed, eye);
	if (cameraDownPressed) eye = bx::mad(up, -deltaTime * moveSpeed, eye);

	at = bx::add(eye, direction);
	m_up = bx::cross(right, direction);
}

void FlyCamera::setPosition(const bx::Vec3& pos) {
	eye = pos;
}

void FlyCamera::setVerticalAngle(float angle) {
	verticalAngle = angle;
}

void FlyCamera::setHorizontalAngle(float angle) {
	horizontalAngle = angle;
}

void FlyCamera::getViewMtx(float* viewMtx) {
	bx::mtxLookAt(viewMtx, bx::load<bx::Vec3>(&eye.x), bx::load<bx::Vec3>(&at.x), bx::load<bx::Vec3>(&m_up.x) );
}

}
