#ifndef SHADOW_NATIVE_CAMERA_H
#define SHADOW_NATIVE_CAMERA_H

#include <bx/math.h>
#include "Util.h"

namespace Shadow {

	class Camera {
		public:
			Camera();
			~Camera();

			Camera(const Camera &) = delete;
			Camera &operator=(const Camera &) = delete;

			void setPosition(const bx::Vec3& _pos);
			void setVerticalAngle(float _verticalAngle);
			void setHorizontalAngle(float _horizontalAngle);
			void getViewMtx(float* _viewMtx);
			void update(float _deltaTime, const Utilities::MouseState& _mouseState, bool _reset = false);

		private:
			struct MouseCoords {
				int32_t m_mx;
				int32_t m_my;
				int32_t m_mz;
			};

			MouseCoords m_mouseNow;
			MouseCoords m_mouseLast;

			bx::Vec3 m_eye = bx::InitZero;
			bx::Vec3 m_at  = bx::InitZero;
			bx::Vec3 m_up  = bx::InitZero;
			float m_horizontalAngle;
			float m_verticalAngle;

			float m_mouseSpeed;
			float m_moveSpeed;

			uint8_t m_keys;
			bool m_mouseDown;
	};

} // Shadow

#endif //SHADOW_NATIVE_CAMERA_H
