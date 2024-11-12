#ifndef SHADOW_NATIVE_DEBUG_LOGGER_HPP
#define SHADOW_NATIVE_DEBUG_LOGGER_HPP

// General purpose logging tool for Shadow Engine
// to make sure that general strings aren't shown
// in production and to show the original caller.

#include <cstdarg>
#include <cstdio>
#include <inttypes.h>
#include <iostream>
#include <termcolor.hpp>

// Don't want to use BX_UNUSED here because Logger should be independent
#define SHADOW_NATIVE_LOGGER_UNUSED(expression) (void)(true ? (void)0 : ((void)(expression)))

#define PRINT(fmt, ...) Shadow::Logger::debugPrintf(__FILE_NAME__, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) Shadow::Logger::debugWarnf(__FILE_NAME__, fmt, ##__VA_ARGS__)
#define ERROUT(fmt, ...) Shadow::Logger::debugErrorf(__FILE_NAME__, fmt, ##__VA_ARGS__)

namespace Shadow::Logger {

inline void debugPrintfVargs(const char* fmt, va_list argList) {
	char temp[8192];
	char* out = temp;
	int32_t len = vsnprintf(out, sizeof(temp), fmt, argList);
	if ((int32_t)sizeof(temp) < len) {
		out = (char*)alloca(len + 1);
		len = vsnprintf(out, len, fmt, argList);
	}
	out[len] = '\0';
	std::cout << out << std::endl;
}

inline void debugPrintf(const char* caller, const char* fmt, ...) {
#ifdef SHADOW_DEBUG_BUILD
	std::cout << "[" << caller << "] ";
	va_list argList;
	va_start(argList, fmt);
	debugPrintfVargs(fmt, argList);
	va_end(argList);
#else
	SHADOW_NATIVE_LOGGER_UNUSED(caller);
	SHADOW_NATIVE_LOGGER_UNUSED(fmt);
#endif
}

inline void debugWarnf(const char* caller, const char* fmt, ...) {
#ifdef SHADOW_DEBUG_BUILD
	std::cout << termcolor::yellow << "WARN: {" << caller << "} " << termcolor::reset;
	va_list argList;
	va_start(argList, fmt);
	debugPrintfVargs(fmt, argList);
	va_end(argList);
#else
	SHADOW_NATIVE_LOGGER_UNUSED(caller);
	SHADOW_NATIVE_LOGGER_UNUSED(fmt);
#endif
}

inline void debugErrorf(const char* caller, const char* fmt, ...) {
#ifdef SHADOW_DEBUG_BUILD
	std::cerr << termcolor::red << "ERROR: {" << caller << "} " << termcolor::reset;
	va_list argList;
	va_start(argList, fmt);
	debugPrintfVargs(fmt, argList);
	va_end(argList);
#else
	SHADOW_NATIVE_LOGGER_UNUSED(caller);
	SHADOW_NATIVE_LOGGER_UNUSED(fmt);
#endif
}

}

#endif /* SHADOW_NATIVE_DEBUG_LOGGER_HPP */
