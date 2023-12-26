#ifndef SHADOW_NATIVE_KEYBOARD_HPP
#define SHADOW_NATIVE_KEYBOARD_HPP

#include "GLFW/glfw3.h"
#include "ShadowWindow.hpp"
#include <vector>

namespace Shadow {

enum KeyButton_ {
	KeyButton_None = 0,
	KeyButton_Space,
	KeyButton_Apostrophe,
	KeyButton_Comma,
	KeyButton_Minus,
	KeyButton_Period,
	KeyButton_ForwardSlash,
	KeyButton_0,
	KeyButton_1,
	KeyButton_2,
	KeyButton_3,
	KeyButton_4,
	KeyButton_5,
	KeyButton_6,
	KeyButton_7,
	KeyButton_8,
	KeyButton_9,
	KeyButton_Semicolon,
	KeyButton_Equal,
	KeyButton_A,
	KeyButton_B,
	KeyButton_C,
	KeyButton_D,
	KeyButton_E,
	KeyButton_F,
	KeyButton_G,
	KeyButton_H,
	KeyButton_I,
	KeyButton_J,
	KeyButton_K,
	KeyButton_L,
	KeyButton_M,
	KeyButton_N,
	KeyButton_O,
	KeyButton_P,
	KeyButton_Q,
	KeyButton_R,
	KeyButton_S,
	KeyButton_T,
	KeyButton_U,
	KeyButton_V,
	KeyButton_W,
	KeyButton_X,
	KeyButton_Y,
	KeyButton_Z,
	KeyButton_LeftBracket,
	KeyButton_BackSlash,
	KeyButton_RightBracket,
	KeyButton_GraveAccent,
	KeyButton_World1,
	KeyButton_World2,
	KeyButton_Escape,
	KeyButton_Enter,
	KeyButton_Tab,
	KeyButton_Backspace,
	KeyButton_Insert,
	KeyButton_Delete,
	KeyButton_Right,
	KeyButton_Left,
	KeyButton_Down,
	KeyButton_Up,
	KeyButton_PageUp,
	KeyButton_PageDown,
	KeyButton_Home,
	KeyButton_End,
	KeyButton_CapsLock,
	KeyButton_ScrollLock,
	KeyButton_NumLock,
	KeyButton_PrintScreen,
	KeyButton_Pause,
	KeyButton_F1,
	KeyButton_F2,
	KeyButton_F3,
	KeyButton_F4,
	KeyButton_F5,
	KeyButton_F6,
	KeyButton_F7,
	KeyButton_F8,
	KeyButton_F9,
	KeyButton_F10,
	KeyButton_F11,
	KeyButton_F12,
	KeyButton_F13,
	KeyButton_F14,
	KeyButton_F15,
	KeyButton_F16,
	KeyButton_F17,
	KeyButton_F18,
	KeyButton_F19,
	KeyButton_F20,
	KeyButton_F21,
	KeyButton_F22,
	KeyButton_F23,
	KeyButton_F24,
	KeyButton_F25,
	KeyButton_KeyPad_0,
	KeyButton_KeyPad_1,
	KeyButton_KeyPad_2,
	KeyButton_KeyPad_3,
	KeyButton_KeyPad_4,
	KeyButton_KeyPad_5,
	KeyButton_KeyPad_6,
	KeyButton_KeyPad_7,
	KeyButton_KeyPad_8,
	KeyButton_KeyPad_9,
	KeyButton_KeyPad_Decimal,
	KeyButton_KeyPad_Divide,
	KeyButton_KeyPad_Multiply,
	KeyButton_KeyPad_Subtract,
	KeyButton_KeyPad_Add,
	KeyButton_KeyPad_Enter,
	KeyButton_KeyPad_Equal,
	KeyButton_LeftShift,
	KeyButton_LeftControl,
	KeyButton_LeftAlt,
	KeyButton_LeftSuper,
	KeyButton_RightShift,
	KeyButton_RightControl,
	KeyButton_RightAlt,
	KeyButton_RightSuper,
	KeyButton_Menu
};

typedef void (* keyFunction)(KeyButton_ key, bool down);

class Keyboard {
public:
	Keyboard(ShadowWindow* window);
	~Keyboard();

	// void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void registerKeyCallback(keyFunction callback);

	std::vector<keyFunction> keyListeners;
	KeyButton_ glfwButtonMap[GLFW_KEY_LAST + 1];
private:
	ShadowWindow* window;
};

}

#endif /* SHADOW_NATIVE_KEYBOARD_HPP */