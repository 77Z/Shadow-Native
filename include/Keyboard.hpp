#ifndef SHADOW_NATIVE_KEYBOARD_HPP
#define SHADOW_NATIVE_KEYBOARD_HPP

#include "ShadowWindow.hpp"

namespace Shadow {

class Keyboard {
public:
	Keyboard(ShadowWindow* window);
	~Keyboard();

private:
	ShadowWindow* window;
};

}

#endif /* SHADOW_NATIVE_KEYBOARD_HPP */