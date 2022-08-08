#include <string>
#include "Logger.h"
#include "UserCode.h"
#include <iostream>

#ifdef _WIN32
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

int Shadow::UserCode::loadUserCode(std::string libraryFile) {

	print("Starting to load UserCode");

#ifdef _WIN32
	// https://docs.microsoft.com/en-us/windows/win32/dlls/using-run-time-dynamic-linking

	//TODO: I got no clue if this works, yet to be tested
	HINSTANCE hinstlib = LoadLibrary(TEXT(libraryFile));
#else
	void* handle = dlopen(libraryFile.c_str(), RTLD_LAZY);

	if (!handle) {
		errout("Failed to get handle on UserCode library");
		errout("See next line for more info");
		std::cerr << dlerror() << std::endl;
		return USER_CODE_FAILURE;
	}

	void (*usrcodefunc_start)(void);
	*(void**)(&usrcodefunc_start) = dlsym(handle, "Start");
	if (!usrcodefunc_start) {
		// Symbol doesn't exist
		errout("Symbol 'Start' doesn't exist in UserCode");
		errout("See next line for more info");
		std::cerr << dlerror() << std::endl;
		dlclose(handle);
		return USER_CODE_FAILURE;
	}

	usrcodefunc_start();

	dlclose(handle);

#endif

	return USER_CODE_SUCCESS;
}
