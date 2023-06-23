#ifndef SHADOW_NATIVE_CAMERA_H
#define SHADOW_NATIVE_CAMERA_H

#include "Util.h"
#include <bx/math.h>

namespace Shadow {

class Camera {
public:
	Camera();
	~Camera();

	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;

	void init(const bx::Vec3& _center, float _distance, float _near, float _far);
	void mtxLookAt(float* _outViewMtx);
	void orbit(float _dx, float _dy);
	void distance(float _z);
	void dolly(float _dz);
	void consumeOrbit(float _amount);
	void update(float _dt);

private:
	struct Interp3f {
		bx::Vec3 curr = bx::InitNone;
		bx::Vec3 dest = bx::InitNone;
	};

	Interp3f m_target;
	Interp3f m_pos;
	float m_orbit[2];
	float m_near;
	float m_far;
};

} // namespace Shadow

#endif // SHADOW_NATIVE_CAMERA_H
