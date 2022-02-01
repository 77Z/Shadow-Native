#include <stdio.h>
#include <openvr.h>
#include <stdlib.h>

int main() {

	if (!vr::VR_IsHmdPresent()) {
		fprintf(stderr, "NO HMD CONNECTED!\n");
	}

	//vr::IVRSystem *vr::VR_Init();

	printf("Hello, World!\n");

	vr::VR_Shutdown();

}