#ifndef SHADOW_NATIVE_SHADOW_WINDOW_HPP
#define SHADOW_NATIVE_SHADOW_WINDOW_HPP

#include <GLFW/glfw3.h>
#include <string>

namespace Shadow {
class ShadowWindow {
public:
	ShadowWindow(int width, int height, std::string title, bool decorations = true);
	~ShadowWindow();

	ShadowWindow(const ShadowWindow&) = delete;
	ShadowWindow& operator=(const ShadowWindow&) = delete;

	struct WindowDimensions {
		int width;
		int height;
	};

	bool isMaximized() { return (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED); }
	void maximize() { glfwMaximizeWindow(window); }
	void restore() { glfwRestoreWindow(window); }
	void toggleMaximized() {
		if (isMaximized()) {
			restore();
		} else {
			maximize();
		}
	}

	void minimize() { glfwIconifyWindow(window); }

	bool shouldClose() { return glfwWindowShouldClose(window); }
	void close() { glfwSetWindowShouldClose(window, GLFW_TRUE); }

	WindowDimensions getExtent() { return { width, height }; };
	bool wasWindowResized() { return framebufferResized; }
	void resetWindowResizedFlag() { framebufferResized = false; }
	void shutdown();
	void* getNativeWindowHandle();
	void* getNativeDisplayHandle();

	std::string windowTitle;

	// TODO: Make private
	GLFWwindow* window;

private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	static void glfw_errorCallback(int error, const char* description);
	// static void mouseInputPassthrough(GLFWwindow* window, int button, int action, int mods);
	void initWindow();

	int width;
	int height;
	bool framebufferResized = false;
	bool decorations = true;
};
}

#endif /* SHADOW_NATIVE_SHADOW_WINDOW_HPP */