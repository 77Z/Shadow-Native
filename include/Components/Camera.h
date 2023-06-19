//
// Created by Vince on 6/11/22.
//

#ifndef SHADOW_NATIVE_CAMERA_H
#define SHADOW_NATIVE_CAMERA_H

#include <bx/math.h>

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
			void update(float _deltaTime, int _mouseState/* const entry::MouseState& _mouseState */, bool _reset = false);
	};

} // Shadow

#endif //SHADOW_NATIVE_CAMERA_H
