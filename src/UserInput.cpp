#include "UserInput.h"
#include "Logger.h"
#include <GLFW/glfw3.h>

// * This isn't a class due to some issues I
// * was having with "Invalid use of member []
// * in static member function" BLAH BLAH

GLFWwindow* windowHandle;

bool leftMouseDown = false;
bool rightMouseDown = false;

double lastMouseX, lastMouseY;
double mouseX = 0.0f;
double mouseY = 0.0f;
double mouseXdiff;
double mouseYdiff;

void Shadow::UserInput::initMouse(GLFWwindow* window) { windowHandle = window; }
void Shadow::UserInput::updateMouse() {
	glfwGetCursorPos(windowHandle, &mouseX, &mouseY);
	mouseXdiff = mouseX - lastMouseX;
	mouseYdiff = mouseY - lastMouseY;

	lastMouseX = mouseX;
	lastMouseY = mouseY;
}

bool Shadow::UserInput::isLeftMouseDown() { return leftMouseDown; }
bool Shadow::UserInput::isRightMouseDown() { return rightMouseDown; }

double Shadow::UserInput::getMouseX() { return mouseX; }
double Shadow::UserInput::getMouseY() { return mouseY; }

double Shadow::UserInput::getMouseXDiff() { return mouseXdiff; }
double Shadow::UserInput::getMouseYDiff() { return mouseYdiff; }

void Shadow::UserInput::mouseCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS)
			leftMouseDown = true;
		else if (action == GLFW_RELEASE)
			leftMouseDown = false;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS)
			rightMouseDown = true;
		else if (action == GLFW_RELEASE)
			rightMouseDown = false;
	}
}

// namespace Shadow {

// namespace UserInput {

// 	// void Keyboard::glfw_keyCallback(
// 	// 	GLFWwindow* window, int key, int scancode, int action, int mods) { }

// }

// }
