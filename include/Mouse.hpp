#ifndef SHADOW_NATIVE_MOUSE
#define SHADOW_NATIVE_MOUSE

#include "ShadowWindow.hpp"
#include <cstdint>

namespace Shadow {

struct MouseButton {
	enum Enum {
		None,
		Left,
		Middle,
		Right,

		Count
	};
};

struct MouseState {

	int32_t mx;
	int32_t my;
	int32_t mz;
	uint8_t buttons[MouseButton::Count];

	MouseState()
		: mx(0)
		, my(0)
		, mz(0)
		{
			//TODO: I have no idea what this does!
			for (uint32_t ii = 0; ii < MouseButton::Count; ii++) {
				buttons[ii] = MouseButton::None;
			}
		}
};

class Mouse {
public:

	Mouse(ShadowWindow* window);
	~Mouse();

	bool isLeftMouseDown();
	bool isRightMouseDown();
	double getMouseX() { return mouseX; }
	double getMouseY() { return mouseY; }
	double getMouseXDiff() { return mouseXdiff; }
	double getMouseYDiff() { return mouseYdiff; }

	void onUpdate();
private:

	ShadowWindow* window;

	double lastMouseX, lastMouseY;
	double mouseX = 0.0f;
	double mouseY = 0.0f;
	double mouseXdiff;
	double mouseYdiff;
};

}

#endif /* SHADOW_NATIVE_MOUSE */