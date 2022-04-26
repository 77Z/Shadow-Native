#include <stdio.h>
#include <iostream>
#include <openvr.h>
#include <stdlib.h>

int main() {

	if (!vr::VR_IsHmdPresent()) {
		fprintf(stderr, "NO HMD CONNECTED!\n");
	}

    vr::EVRInitError err = vr::VRInitError_None;
	vr::IVRSystem* hmd = vr::VR_Init(&err, vr::VRApplication_Scene);


	printf("Hello, World!\n");

	vr::VR_Shutdown();

}