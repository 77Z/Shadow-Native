#include "Debug/Logger.hpp"
#include "Util.hpp"
#include "bx/bx.h"
#include "bx/constants.h"
#include <Components/Camera.h>

#include <bx/math.h>

namespace Shadow {

Camera::Camera() { init(bx::InitZero, 2.0f, 0.01f, 100.0f); }
Camera::~Camera() { PRINT("Camera destroyed"); }

void Camera::init(const bx::Vec3& _center, float _distance, float _near, float _far) {
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

void Camera::mtxLookAt(float* _outViewMtx) {
	bx::mtxLookAt(_outViewMtx, m_pos.curr, m_target.curr);
}

void Camera::orbit(float _dx, float _dy) {
	m_orbit[0] += _dx;
	m_orbit[1] += _dy;
}

void Camera::distance(float _z) {
	_z = bx::clamp(_z, m_near, m_far);

	bx::Vec3 toTarget = bx::sub(m_target.dest, m_pos.dest);
	bx::Vec3 toTargetNorm = bx::normalize(toTarget);

	m_pos.dest = bx::mad(toTargetNorm, -_z, m_target.dest);
}

void Camera::dolly(float _dz) {
	const bx::Vec3 toTarget = bx::sub(m_target.dest, m_pos.dest);
	const float toTargetLen = bx::length(toTarget);
	const float invToTargetLen = 1.0f / (toTargetLen + bx::kFloatSmallest);
	const bx::Vec3 toTargetNorm = bx::mul(toTarget, invToTargetLen);

	float delta = toTargetLen * _dz;
	float newLen = toTargetLen - delta;

	if ((m_near < newLen || _dz < 0.0f) && (newLen < m_far || _dz > 0.0f)) {
		m_pos.dest = bx::mad(toTargetNorm, delta, m_pos.dest);
	}
}

void Camera::consumeOrbit(float _amount) {
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

void Camera::update(float _dt) {
	const float amount = bx::min(_dt / 0.12f, 1.0f);

	consumeOrbit(amount);

	m_target.curr = bx::lerp(m_target.curr, m_target.dest, amount);
	m_pos.curr = bx::lerp(m_pos.curr, m_pos.dest, amount);
}

} // namespace Shadow
