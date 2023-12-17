#include "Keyboard.hpp"
#include "GLFW/glfw3.h"
#include "ShadowWindow.hpp"

namespace Shadow {

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		
	}

	Keyboard::Keyboard(ShadowWindow* window): window(window) {
		glfwSetKeyCallback(window->window, keyCallback);
	}
	 
	Keyboard::~Keyboard() {}

}