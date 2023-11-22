#include <cuda.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

__global__ void c_hello() { printf("Hello, World!\n"); }

int main() {
	while (true) {
		c_hello<<<1, 1>>>();
		sleep(1);
	}

	cudaDeviceSynchronize();
}
