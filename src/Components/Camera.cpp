//
// Created by Vince on 6/11/22.
//

#include "Logger.h"
#include "Util.h"
#include <Components/Camera.h>

#include <bx/math.h>

namespace Shadow {

	

	Camera::Camera() {
		WARN("Camera initialized");

		// reset();
		Utilities::MouseState mouseState;
		update(0.0f, mouseState, true);
	}

	Camera::~Camera() {
		WARN("Camera Destroyed!");
	}

	void Camera::update(float _deltaTime, const Utilities::MouseState &_mouseState, bool _reset) {
		if (_reset) {
			m_mouseLast.m_mx = _mouseState.m_mx;
			m_mouseLast.m_my = _mouseState.m_my;
			m_mouseLast.m_mz = _mouseState.m_mz;
			m_mouseNow  = m_mouseLast;
			m_mouseDown = false;

			return;
		}

		if (!m_mouseDown) {
			m_mouseLast.m_mx = _mouseState.m_mx;
			m_mouseLast.m_my = _mouseState.m_my;
		}

		m_mouseDown = !!_mouseState.m_buttons[Utilities::MouseButton::Right];

		if (m_mouseDown) {
			m_mouseNow.m_mx = _mouseState.m_mx;
			m_mouseNow.m_my = _mouseState.m_my;
		}

		m_mouseLast.m_mz = m_mouseNow.m_mz;
		m_mouseNow.m_mz  = _mouseState.m_mz;

		const float deltaZ = float(m_mouseNow.m_mz - m_mouseLast.m_mz);

		if (m_mouseDown) {
			const int32_t deltaX = m_mouseNow.m_mx - m_mouseLast.m_mx;
			const int32_t deltaY = m_mouseNow.m_my - m_mouseLast.m_my;

			m_horizontalAngle += m_mouseSpeed * float(deltaX);
			m_verticalAngle   -= m_mouseSpeed * float(deltaY);

			m_mouseLast.m_mx = m_mouseNow.m_mx;
			m_mouseLast.m_my = m_mouseNow.m_my;
		}
	}

} // Shadow
