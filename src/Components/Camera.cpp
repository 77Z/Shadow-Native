//
// Created by Vince on 6/11/22.
//

#include "Logger.h"
#include "Util.h"
#include "bx/bx.h"
#include "bx/constants.h"
#include <Components/Camera.h>

#include <bx/math.h>

namespace Shadow {

struct Camera {
	Camera() { init(bx::InitZero, 2.0f, 0.01f, 100.0f); }

	void init(const bx::Vec3& _center, float _distance, float _near, float _far)
	{
		m_target.curr = _center;
		m_target.dest = _center;

		m_pos.curr = _center;
		m_pos.curr.z += _distance;
		m_pos.dest = _center;
		m_pos.dest.z += _distance;

		m_orbit[0] = 0.0f;
		m_orbit[1] = 0.0f;

		m_near = _near;
		m_far = _far;
	}

	void mtxLookAt(float* _outViewMtx) { bx::mtxLookAt(_outViewMtx, m_pos.curr, m_target.curr); }

	void orbit(float _dx, float _dy)
	{
		m_orbit[0] += _dx;
		m_orbit[1] += _dy;
	}

	void distance(float _z)
	{
		_z = bx::clamp(_z, m_near, m_far);

		bx::Vec3 toTarget = bx::sub(m_target.dest, m_pos.dest);
		bx::Vec3 toTargetNorm = bx::normalize(toTarget);

		m_pos.dest = bx::mad(toTargetNorm, -_z, m_target.dest);
	}

	void consumeOrbit(float _amount)
	{
		float consume[2];
		consume[0] = m_orbit[0] * _amount;
		consume[1] = m_orbit[1] * _amount;
		m_orbit[0] -= consume[0];
		m_orbit[1] -= consume[1];

		const bx::Vec3 toPos = bx::sub(m_pos.curr, m_target.curr);
		const float toPosLen = bx::length(toPos);
		const float invToPosLen = 1.0f / (toPosLen + bx::kFloatSmallest);
		const bx::Vec3 toPosNorm = bx::mul(toPos, invToPosLen);

		float ll[2];
		bx::toLatLong(&ll[0], &ll[1], toPosNorm);
		ll[0] += consume[0];
		ll[1] -= consume[1];
		ll[1] = bx::clamp(ll[1], 0.02f, 0.98f);

		const bx::Vec3 tmp = bx::fromLatLong(ll[0], ll[1]);
		const bx::Vec3 diff = bx::mul(bx::sub(tmp, toPosNorm), toPosLen);

		m_pos.curr = bx::add(m_pos.curr, diff);
		m_pos.dest = bx::add(m_pos.dest, diff);
	}

	void update(float _dt)
	{
		const float amount = bx::min(_dt / 0.12f, 1.0f);

		consumeOrbit(amount);

		m_target.curr = bx::lerp(m_target.curr, m_target.dest, amount);
		m_pos.curr = bx::lerp(m_pos.curr, m_pos.dest, amount);
	}

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
