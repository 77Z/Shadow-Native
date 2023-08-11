#include "UserCode.hpp"
#include "Debug/Logger.hpp"
#include <fstream>
#include <generated/autoconf.h>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

// void callme() { WARN("I HAVE BEEN CALLED"); }

int Shadow::UserCode::loadUserCode(void) {

	std::string libraryFile = CONFIG_DYNAMIC_USERCODE_LIBRARY_LOCATION;

	PRINT("Starting to load UserCode");

	std::ifstream libFile(libraryFile);
	if (!libFile) {
		// UserCode doesn't exist, throw warning
		WARN("UserCode Library doesn't exist! Shadow will continue without it");
		libFile.close();
		return UserCodeReturnStatus_Failure;
	}
	libFile.close();

	// function registration from within UserCode Lib
	void (*register_function)(void (*)());

	void* handle = dlopen(libraryFile.c_str(), RTLD_LAZY);

	if (!handle) {
		ERROUT("Failed to get handle on UserCode library ... %s", dlerror());
		return UserCodeReturnStatus_Failure;
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
		return UserCodeReturnStatus_Failure;
	}

	/* *(void**)(&register_function) = dlsym(handle, "register_function");
	if (!register_function) {
		ERROUT("Symbol for function registration and callbacks doesn't exist in UserCode library! "
			   "... %s",
			dlerror());
		dlclose(handle);
		return USER_CODE_FAILURE;
	}
	register_function(callme); // Allow usage of this function */

	usrcodefunc_start();

	dlclose(handle);

	return UserCodeReturnStatus_Success;
}