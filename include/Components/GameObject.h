#ifndef SHADOW_NATIVE_GAME_OBJECT_H
#define SHADOW_NATIVE_GAME_OBJECT_H

#include "bx/math.h"
namespace Shadow {
class GameObject {
public:
	GameObject();
	~GameObject();

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	bx::Vec3 pos = bx::Vec3(0.0f, 0.0f, 0.0f);
	bx::Vec3 scale = bx::Vec3(0.0f, 0.0f, 0.0f);
	bx::Vec3 rotation = bx::Vec3(0.0f, 0.0f, 0.0f);
};
}

#endif /* SHADOW_NATIVE_GAME_OBJECT_H */