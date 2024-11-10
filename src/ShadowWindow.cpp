#include "ShadowWindow.hpp"
#include "Debug/Logger.hpp"
#include "ShadowWindowUserPointerCarePackage.hpp"
#include "bx/platform.h"
#include <vector>

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX

#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL

#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Shadow {

ShadowWindow::ShadowWindow(
	int width, int height, std::string title, bool decorations, bool openGlAPI, bool fullscreen)
	: width(width)
	, height(height)
	, windowTitle(title)
	, decorations(decorations)
	, openGlAPI(openGlAPI)
	, fullscreen(fullscreen) {
	initWindow();
}

ShadowWindow::~ShadowWindow() { }

void ShadowWindow::shutdown() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void* ShadowWindow::getNativeWindowHandle() {
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	return (void*)(uintptr_t)glfwGetX11Window(window);
#elif BX_PLATFORM_WINDOWS
	return glfwGetWin32Window(window);
#elif BX_PLATFORM_OSX
	return glfwGetCocoaWindow(window);
#endif
}

void* ShadowWindow::getNativeDisplayHandle() {
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	return glfwGetX11Display();
#else
	return NULL;
#endif
}

float ShadowWindow::getContentScale() {
	float s;
	glfwGetWindowContentScale(window, &s, nullptr);
	return s;
}

void ShadowWindow::initWindow() {
	glfwSetErrorCallback(glfw_errorCallback);

	if (!glfwInit()) ERROUT("GLFW FAILED TO INIT, EXPECT FAILURE FROM HERE");
	if (!openGlAPI) {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, (int)decorations);

	GLFWmonitor* primary = glfwGetPrimaryMonitor();

	window = glfwCreateWindow(width, height, windowTitle.c_str(), fullscreen ? primary : nullptr, nullptr);

	windowUserPointers["window"] = this;
	glfwSetWindowUserPointer(window, &windowUserPointers);

	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void ShadowWindow::glfw_errorCallback(int error, const char* description) {
	ERROUT("GLFW error %d: %s\n", error, description);
}

void ShadowWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto userPointers = reinterpret_cast<UserPointersMap*>(glfwGetWindowUserPointer(window));
	ShadowWindow* shadowWindow = static_cast<ShadowWindow*>(userPointers->at("window"));
	shadowWindow->framebufferResized = true;
	shadowWindow->width = width;
	shadowWindow->height = height;
}

}
