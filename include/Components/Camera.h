//
// Created by Vince on 6/11/22.
//

#ifndef SHADOW_NATIVE_CAMERA_H
#define SHADOW_NATIVE_CAMERA_H

#include <bx/math.h>

namespace Shadow {

	struct Camera {
		Camera();
		void init(const bx::Vec3& _center, float _distance, float _near, float _far);
		void mtxLookAt(float* _outViewMtx);
		void orbit(float _dx, float _dy);
		void distance(float _z);
		void dolly(float _dz);
		void consumeOrbit(float _amount);
		void update(float _dt);

		struct Interp3f {
			bx::Vec3 curr = bx::init::None;
			bx::Vec3 dest = bx::init::None;
		};

		Interp3f m_target;
		Interp3f m_pos;
		float m_orbit[2];
		float m_near, m_far;
	};

} // Shadow

#endif //SHADOW_NATIVE_CAMERA_H
