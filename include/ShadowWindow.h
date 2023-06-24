#ifndef SHADOW_NATIVE_SHADOW_WINDOW_H
#define SHADOW_NATIVE_SHADOW_WINDOW_H

#include <GLFW/glfw3.h>
#include <string>

namespace Shadow {
class ShadowWindow {
public:
	ShadowWindow(int width, int height, std::string title);
	~ShadowWindow();

	ShadowWindow(const ShadowWindow&) = delete;
	ShadowWindow& operator=(const ShadowWindow&) = delete;

	struct WindowDimensions {
		int width;
		int height;
	};

	bool shouldClose() { return glfwWindowShouldClose(window); }
	WindowDimensions getExtent() { return { width, height }; };
	bool wasWindowResized() { return framebufferResized; }
	void resetWindowResizedFlag() { framebufferResized = false; }

private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	static void glfw_errorCallback(int error, const char* description);
	void initWindow();

	int width;
	int height;
	bool framebufferResized = false;

	std::string windowTitle;
	GLFWwindow* window;
};
}

#endif