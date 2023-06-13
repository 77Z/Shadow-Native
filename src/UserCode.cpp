#include <string>
#include "Logger.h"
#include "UserCode.h"
#include <fstream>

#ifdef _WIN32
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

void callme() {
	WARN("I HAVE BEEN CALLED");
}

int Shadow::UserCode::loadUserCode(std::string libraryFile) {

#ifdef CONFIG_DYNAMIC_USERCODE_LIBRARY

	PRINT("Starting to load UserCode");

#	ifdef _WIN32
	// https://docs.microsoft.com/en-us/windows/win32/dlls/using-run-time-dynamic-linking

	//TODO: I got no clue if this works, yet to be tested
	HINSTANCE hinstlib = LoadLibrary(TEXT(libraryFile));
#	else

	std::ifstream libFile(libraryFile);
	if (!libFile) {
		// UserCode doesn't exist, throw warning
		WARN("UserCode Library doesn't exist! Shadow will continue without it");
		libFile.close();
		return USER_CODE_FAILURE;
	}
	libFile.close();

	// function registration from within UserCode Lib
	void (*register_function)(void(*)());

	void* handle = dlopen(libraryFile.c_str(), RTLD_LAZY);

	if (!handle) {
		ERROUT("Failed to get handle on UserCode library ... %s", dlerror());
		return USER_CODE_FAILURE;
	}
	/*
	 Return type           Argument Types
	 ||                        ||
	 \/                        \/		*/
	void (*usrcodefunc_start)(void);
	*(void**)(&usrcodefunc_start) = dlsym(handle, "passthroughStart");
	if (!usrcodefunc_start) {
		// Symbol doesn't exist
		ERROUT("Symbol doesn't exist in UserCode ... %s", dlerror());
		dlclose(handle);
		return USER_CODE_FAILURE;
	}


	*(void**)(&register_function) = dlsym(handle, "register_function");
	if (!register_function) {
		ERROUT("Symbol for function registration and callbacks doesn't exist in UserCode library! ... %s", dlerror());
		dlclose(handle);
		return USER_CODE_FAILURE;
	}
	register_function(callme); // Allow usage of this function

	usrcodefunc_start();

	dlclose(handle);

#	endif

#else
	WARN("UserCode disabled in config");
#endif
	return USER_CODE_SUCCESS;
}
