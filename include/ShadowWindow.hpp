#ifndef SHADOW_NATIVE_SHADOW_WINDOW_HPP
#define SHADOW_NATIVE_SHADOW_WINDOW_HPP

#include <GLFW/glfw3.h>
#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, void*> UserPointersMap;

namespace Shadow {
class ShadowWindow {
public:
  ShadowWindow(int width, int height, std::string title, bool decorations = true, bool openGlAPI = false, bool fullscreen = false);
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

	/// Polls contstantly, good for games
	void pollEvents() { glfwPollEvents(); }

	/// Polls on user input, good for UI tools
	void waitEvents() { glfwWaitEvents(); }
	/// Post an empty event to update the screen
	void postEmptyEvent() { glfwPostEmptyEvent(); }

	WindowDimensions getExtent() { return { width, height }; };
	bool wasWindowResized() { return framebufferResized; }
	void resetWindowResizedFlag() { framebufferResized = false; }
	void shutdown();
	void* getNativeWindowHandle();
	void* getNativeDisplayHandle();
	float getContentScale();

	void lockCursor() { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
	void unlockCursor() { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }

	std::string windowTitle;

	// std::vector<void*> windowUserPointers;
	// ShadowWindowUserPointerCarePackage userPointerCarePackage;
	UserPointersMap windowUserPointers;

	/// Access underlying GLFWwindow handle
	GLFWwindow* window;

private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	static void glfw_errorCallback(int error, const char* description);
	static void glfw_dropCallback(GLFWwindow* window, int count, const char** paths);
	// static void mouseInputPassthrough(GLFWwindow* window, int button, int action, int mods);
	void initWindow();

	int width;
	int height;
	bool framebufferResized = false;
	bool decorations = true;
	bool fullscreen = false;
	bool openGlAPI = false;
};
}

#endif /* SHADOW_NATIVE_SHADOW_WINDOW_HPP */
