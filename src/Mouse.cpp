#include "Mouse.hpp"
#include "Debug/Logger.hpp"
#include "GLFW/glfw3.h"
#include "ShadowWindow.hpp"
#include "bx/bx.h"

namespace Shadow {

static bool localLeftMouseDown = false;
static bool localRightMouseDown = false;

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	BX_UNUSED(window);
	BX_UNUSED(mods);

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS)
			localLeftMouseDown = true;
		else if (action == GLFW_RELEASE)
			localLeftMouseDown = false;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS)
			localRightMouseDown = true;
		else if (action == GLFW_RELEASE)
			localRightMouseDown = false;
	}
}

Mouse::Mouse(ShadowWindow* window):window(window) {
	glfwSetMouseButtonCallback(window->window, mouseButtonCallback);
}

Mouse::~Mouse() {}

bool Mouse::isLeftMouseDown() { return localLeftMouseDown; }
bool Mouse::isRightMouseDown() { return localRightMouseDown; }

void Mouse::onUpdate() {
	glfwGetCursorPos(window->window, &mouseX, &mouseY);
	mouseXdiff = mouseX - lastMouseX;
	mouseYdiff = mouseY - lastMouseY;

	lastMouseX = mouseX;
	lastMouseY = mouseY;
}

}