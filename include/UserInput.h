#ifndef SHADOW_NATIVE_USER_INPUT_H
#define SHADOW_NATIVE_USER_INPUT_H

#include <GLFW/glfw3.h>

namespace Shadow {

namespace UserInput {

	void initMouse(GLFWwindow* window);
	void updateMouse();

	bool isLeftMouseDown();
	bool isRightMouseDown();

	double getMouseX();
	double getMouseY();

	double getMouseXDiff();
	double getMouseYDiff();

	void mouseCallback(GLFWwindow* window, int button, int action, int mods);

}

class Keyboard {
public:
	Keyboard();
	~Keyboard();

	void glfw_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

}

#endif /* SHADOW_NATIVE_USER_INPUT_H */