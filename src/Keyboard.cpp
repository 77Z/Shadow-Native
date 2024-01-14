#include "Keyboard.hpp"
#include "Debug/Logger.hpp"
#include "GLFW/glfw3.h"
#include "ShadowWindow.hpp"
#include "bx/bx.h"
#include "ppk_assert_impl.hpp"

namespace Shadow {

	static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		BX_UNUSED(scancode);

		if (key == GLFW_KEY_UNKNOWN) return;

		auto userPointers = reinterpret_cast<UserPointersMap*>(glfwGetWindowUserPointer(window));
		Keyboard* keyboard = static_cast<Keyboard*>(userPointers->at("keyboard"));
		for (auto callback : keyboard->keyListeners) {
			callback(
				keyboard->glfwButtonMap[key],
				action == GLFW_PRESS || action == GLFW_REPEAT
			);
		}
	}

	void Keyboard::registerKeyCallback(keyFunction callback) {
		keyListeners.push_back(callback);
	}

	Keyboard::Keyboard(ShadowWindow* window): window(window) {

		// PPK_ASSERT(window->windowUserPointers["window"] == nullptr, "Keyboard already initialized on this window");

		window->windowUserPointers["keyboard"] = this;

		glfwSetKeyCallback(window->window, glfwKeyCallback);

		glfwButtonMap[GLFW_KEY_SPACE] =			KeyButton_Space;
		glfwButtonMap[GLFW_KEY_APOSTROPHE] =	KeyButton_Apostrophe;
		glfwButtonMap[GLFW_KEY_COMMA] =			KeyButton_Comma;
		glfwButtonMap[GLFW_KEY_MINUS] =			KeyButton_Minus;
		glfwButtonMap[GLFW_KEY_PERIOD] =		KeyButton_Period;
		glfwButtonMap[GLFW_KEY_SLASH] =			KeyButton_ForwardSlash;
		glfwButtonMap[GLFW_KEY_0] =				KeyButton_0;
		glfwButtonMap[GLFW_KEY_1] =				KeyButton_1;
		glfwButtonMap[GLFW_KEY_2] =				KeyButton_2;
		glfwButtonMap[GLFW_KEY_3] =				KeyButton_3;
		glfwButtonMap[GLFW_KEY_4] =				KeyButton_4;
		glfwButtonMap[GLFW_KEY_5] =				KeyButton_5;
		glfwButtonMap[GLFW_KEY_6] =				KeyButton_6;
		glfwButtonMap[GLFW_KEY_7] =				KeyButton_7;
		glfwButtonMap[GLFW_KEY_8] =				KeyButton_8;
		glfwButtonMap[GLFW_KEY_9] =				KeyButton_9;
		glfwButtonMap[GLFW_KEY_SEMICOLON] = 	KeyButton_Semicolon;
		glfwButtonMap[GLFW_KEY_EQUAL] = 		KeyButton_Equal;
		glfwButtonMap[GLFW_KEY_A] = 			KeyButton_A;
		glfwButtonMap[GLFW_KEY_B] = 			KeyButton_B;
		glfwButtonMap[GLFW_KEY_C] = 			KeyButton_C;
		glfwButtonMap[GLFW_KEY_D] = 			KeyButton_D;
		glfwButtonMap[GLFW_KEY_E] = 			KeyButton_E;
		glfwButtonMap[GLFW_KEY_F] = 			KeyButton_F;
		glfwButtonMap[GLFW_KEY_G] = 			KeyButton_G;
		glfwButtonMap[GLFW_KEY_H] = 			KeyButton_H;
		glfwButtonMap[GLFW_KEY_I] = 			KeyButton_I;
		glfwButtonMap[GLFW_KEY_J] = 			KeyButton_J;
		glfwButtonMap[GLFW_KEY_K] = 			KeyButton_K;
		glfwButtonMap[GLFW_KEY_L] = 			KeyButton_L;
		glfwButtonMap[GLFW_KEY_M] = 			KeyButton_M;
		glfwButtonMap[GLFW_KEY_N] = 			KeyButton_N;
		glfwButtonMap[GLFW_KEY_O] = 			KeyButton_O;
		glfwButtonMap[GLFW_KEY_P] = 			KeyButton_P;
		glfwButtonMap[GLFW_KEY_Q] = 			KeyButton_Q;
		glfwButtonMap[GLFW_KEY_R] = 			KeyButton_R;
		glfwButtonMap[GLFW_KEY_S] = 			KeyButton_S;
		glfwButtonMap[GLFW_KEY_T] = 			KeyButton_T;
		glfwButtonMap[GLFW_KEY_U] = 			KeyButton_U;
		glfwButtonMap[GLFW_KEY_V] = 			KeyButton_V;
		glfwButtonMap[GLFW_KEY_W] = 			KeyButton_W;
		glfwButtonMap[GLFW_KEY_X] = 			KeyButton_X;
		glfwButtonMap[GLFW_KEY_Y] = 			KeyButton_Y;
		glfwButtonMap[GLFW_KEY_Z] = 			KeyButton_Z;
		glfwButtonMap[GLFW_KEY_LEFT_BRACKET] = 	KeyButton_LeftBracket;
		glfwButtonMap[GLFW_KEY_BACKSLASH] = 	KeyButton_BackSlash;
		glfwButtonMap[GLFW_KEY_RIGHT_BRACKET] = KeyButton_RightBracket;
		glfwButtonMap[GLFW_KEY_GRAVE_ACCENT] = 	KeyButton_GraveAccent;
		glfwButtonMap[GLFW_KEY_WORLD_1] = 		KeyButton_World1;
		glfwButtonMap[GLFW_KEY_WORLD_2] = 		KeyButton_World2;
		glfwButtonMap[GLFW_KEY_ESCAPE] = 		KeyButton_Escape;
		glfwButtonMap[GLFW_KEY_ENTER] = 		KeyButton_Enter;
		glfwButtonMap[GLFW_KEY_TAB] = 			KeyButton_Tab;
		glfwButtonMap[GLFW_KEY_BACKSPACE] = 	KeyButton_Backspace;
		glfwButtonMap[GLFW_KEY_INSERT] = 		KeyButton_Insert;
		glfwButtonMap[GLFW_KEY_DELETE] = 		KeyButton_Delete;
		glfwButtonMap[GLFW_KEY_RIGHT] = 		KeyButton_Right;
		glfwButtonMap[GLFW_KEY_LEFT] = 			KeyButton_Left;
		glfwButtonMap[GLFW_KEY_DOWN] = 			KeyButton_Down;
		glfwButtonMap[GLFW_KEY_UP] = 			KeyButton_Up;
		glfwButtonMap[GLFW_KEY_PAGE_UP] = 		KeyButton_PageUp;
		glfwButtonMap[GLFW_KEY_PAGE_DOWN] = 	KeyButton_PageDown;
		glfwButtonMap[GLFW_KEY_HOME] = 			KeyButton_Home;
		glfwButtonMap[GLFW_KEY_END] = 			KeyButton_End;
		glfwButtonMap[GLFW_KEY_CAPS_LOCK] = 	KeyButton_CapsLock;
		glfwButtonMap[GLFW_KEY_SCROLL_LOCK] = 	KeyButton_ScrollLock;
		glfwButtonMap[GLFW_KEY_NUM_LOCK] = 		KeyButton_NumLock;
		glfwButtonMap[GLFW_KEY_PRINT_SCREEN] = 	KeyButton_PrintScreen;
		glfwButtonMap[GLFW_KEY_PAUSE] = 		KeyButton_Pause;
		glfwButtonMap[GLFW_KEY_F1] = 			KeyButton_F1;
		glfwButtonMap[GLFW_KEY_F2] = 			KeyButton_F2;
		glfwButtonMap[GLFW_KEY_F3] = 			KeyButton_F3;
		glfwButtonMap[GLFW_KEY_F4] = 			KeyButton_F4;
		glfwButtonMap[GLFW_KEY_F5] = 			KeyButton_F5;
		glfwButtonMap[GLFW_KEY_F6] = 			KeyButton_F6;
		glfwButtonMap[GLFW_KEY_F7] = 			KeyButton_F7;
		glfwButtonMap[GLFW_KEY_F8] = 			KeyButton_F8;
		glfwButtonMap[GLFW_KEY_F9] = 			KeyButton_F9;
		glfwButtonMap[GLFW_KEY_F10] = 			KeyButton_F10;
		glfwButtonMap[GLFW_KEY_F11] = 			KeyButton_F11;
		glfwButtonMap[GLFW_KEY_F12] = 			KeyButton_F12;
		glfwButtonMap[GLFW_KEY_F13] = 			KeyButton_F13;
		glfwButtonMap[GLFW_KEY_F14] = 			KeyButton_F14;
		glfwButtonMap[GLFW_KEY_F15] = 			KeyButton_F15;
		glfwButtonMap[GLFW_KEY_F16] = 			KeyButton_F16;
		glfwButtonMap[GLFW_KEY_F17] = 			KeyButton_F17;
		glfwButtonMap[GLFW_KEY_F18] = 			KeyButton_F18;
		glfwButtonMap[GLFW_KEY_F19] = 			KeyButton_F19;
		glfwButtonMap[GLFW_KEY_F20] = 			KeyButton_F20;
		glfwButtonMap[GLFW_KEY_F21] = 			KeyButton_F21;
		glfwButtonMap[GLFW_KEY_F22] = 			KeyButton_F22;
		glfwButtonMap[GLFW_KEY_F23] = 			KeyButton_F23;
		glfwButtonMap[GLFW_KEY_F24] = 			KeyButton_F24;
		glfwButtonMap[GLFW_KEY_F25] = 			KeyButton_F25;
		glfwButtonMap[GLFW_KEY_KP_0] = 			KeyButton_KeyPad_0;
		glfwButtonMap[GLFW_KEY_KP_1] = 			KeyButton_KeyPad_1;
		glfwButtonMap[GLFW_KEY_KP_2] = 			KeyButton_KeyPad_2;
		glfwButtonMap[GLFW_KEY_KP_3] = 			KeyButton_KeyPad_3;
		glfwButtonMap[GLFW_KEY_KP_4] = 			KeyButton_KeyPad_4;
		glfwButtonMap[GLFW_KEY_KP_5] = 			KeyButton_KeyPad_5;
		glfwButtonMap[GLFW_KEY_KP_6] = 			KeyButton_KeyPad_6;
		glfwButtonMap[GLFW_KEY_KP_7] = 			KeyButton_KeyPad_7;
		glfwButtonMap[GLFW_KEY_KP_8] = 			KeyButton_KeyPad_8;
		glfwButtonMap[GLFW_KEY_KP_9] = 			KeyButton_KeyPad_9;
		glfwButtonMap[GLFW_KEY_KP_DECIMAL] = 	KeyButton_KeyPad_Decimal;
		glfwButtonMap[GLFW_KEY_KP_DIVIDE] = 	KeyButton_KeyPad_Divide;
		glfwButtonMap[GLFW_KEY_KP_MULTIPLY] = 	KeyButton_KeyPad_Multiply;
		glfwButtonMap[GLFW_KEY_KP_SUBTRACT] = 	KeyButton_KeyPad_Subtract;
		glfwButtonMap[GLFW_KEY_KP_ADD] = 		KeyButton_KeyPad_Add;
		glfwButtonMap[GLFW_KEY_KP_ENTER] = 		KeyButton_KeyPad_Enter;
		glfwButtonMap[GLFW_KEY_KP_EQUAL] = 		KeyButton_KeyPad_Equal;
		glfwButtonMap[GLFW_KEY_LEFT_SHIFT] = 	KeyButton_LeftShift;
		glfwButtonMap[GLFW_KEY_LEFT_CONTROL] = 	KeyButton_LeftControl;
		glfwButtonMap[GLFW_KEY_LEFT_ALT] = 		KeyButton_LeftAlt;
		glfwButtonMap[GLFW_KEY_LEFT_SUPER] = 	KeyButton_LeftSuper;
		glfwButtonMap[GLFW_KEY_RIGHT_SHIFT] = 	KeyButton_RightShift;
		glfwButtonMap[GLFW_KEY_RIGHT_CONTROL] = KeyButton_RightControl;
		glfwButtonMap[GLFW_KEY_RIGHT_ALT] = 	KeyButton_RightAlt;
		glfwButtonMap[GLFW_KEY_RIGHT_SUPER] = 	KeyButton_RightSuper;
		glfwButtonMap[GLFW_KEY_MENU] = 			KeyButton_Menu;
	}
	 
	Keyboard::~Keyboard() {}

}