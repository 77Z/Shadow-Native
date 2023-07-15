#include "ShadowWindow.h"
#include "Debug/Logger.h"
#include "UserInput.h"
#include <GLFW/glfw3.h>

static void mouseInputPassthrough(GLFWwindow* window, int button, int action, int mods) {
	Shadow::UserInput::mouseCallback(window, button, action, mods);
}

namespace Shadow {

ShadowWindow::ShadowWindow(int width, int height, std::string title)
	: width(width)
	, height(height)
	, windowTitle(title) {
	initWindow();
}

ShadowWindow::~ShadowWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void ShadowWindow::initWindow() {
	glfwSetErrorCallback(glfw_errorCallback);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

	glfwSetMouseButtonCallback(window, mouseInputPassthrough);
}

void ShadowWindow::glfw_errorCallback(int error, const char* description) {
	ERROUT("GLFW error %d: %s\n", error, description);
}

void ShadowWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto shadowWindow = reinterpret_cast<ShadowWindow*>(glfwGetWindowUserPointer(window));
	shadowWindow->framebufferResized = true;
	shadowWindow->width = width;
	shadowWindow->height = height;
}

}