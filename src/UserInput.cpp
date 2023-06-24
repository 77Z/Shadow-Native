#include "UserInput.h"
#include "Logger.h"
#include <GLFW/glfw3.h>

// * This isn't a class due to some issues I
// * was having with "Invalid use of member []
// * in static member function" BLAH BLAH

bool leftMouseDown = false;
bool rightMouseDown = false;

bool Shadow::UserInput::isLeftMouseDown() { return leftMouseDown; }
bool Shadow::UserInput::isRightMouseDown() { return rightMouseDown; }

double mouseX = 0.0f;
double mouseY = 0.0f;

double Shadow::UserInput::getMouseX() { return mouseX; }
double Shadow::UserInput::getMouseY() { return mouseY; }

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
