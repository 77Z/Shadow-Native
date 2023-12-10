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

Mouse(ShadowWindow& window);
~Mouse();
};

}

#endif /* SHADOW_NATIVE_MOUSE */