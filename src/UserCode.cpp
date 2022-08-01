#include <string>
#include "Logger.h"

#ifdef _WIN32
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

int loadUserCode(std::string libraryFile) {

#ifdef _WIN32
	// https://docs.microsoft.com/en-us/windows/win32/dlls/using-run-time-dynamic-linking
	
	//TODO: I got no clue if this works, yet to be tested
	HINSTANCE hinstlib = LoadLibrary(TEXT(libraryFile));
#else
	void* handle = dlopen(libraryFile.c_str(), RTLD_LAZY);

#endif

}
