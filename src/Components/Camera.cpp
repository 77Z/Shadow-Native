//
// Created by Vince on 6/11/22.
//

#include "Logger.h"
#include <Components/Camera.h>

#include <bx/math.h>

namespace Shadow {

	Camera::Camera() {
		WARN("Camera initialized");
	}

	Camera::~Camera() {
		WARN("Camera Destroyed!");
	}

} // Shadow
