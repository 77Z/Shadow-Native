#ifndef SHADOW_NATIVE_SHADOW_WINDOW_USER_POINTER_CARE_PACKAGE_HPP
#define SHADOW_NATIVE_SHADOW_WINDOW_USER_POINTER_CARE_PACKAGE_HPP

class Keyboard;
class ShadowWindow;

namespace Shadow {

struct ShadowWindowUserPointerCarePackage {
	ShadowWindow* window;
	Keyboard* keyboard;
};

}


#endif /* SHADOW_NATIVE_SHADOW_WINDOW_USER_POINTER_CARE_PACKAGE_HPP */