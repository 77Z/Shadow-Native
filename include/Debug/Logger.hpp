#ifndef SHADOW_NATIVE_DEBUG_LOGGER_HPP
#define SHADOW_NATIVE_DEBUG_LOGGER_HPP

// General purpose logging tool for Shadow Engine
// to make sure that general strings aren't shown
// in production and to show the original caller.

#include <cstdarg>
#include <cstdio>
#include <inttypes.h>
#include <iostream>
#include <string>
#include <termcolor.hpp>

#define PRINT(fmt, ...) Shadow::Logger::debugPrintf(__FILE__, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) Shadow::Logger::debugWarnf(__FILE__, fmt, ##__VA_ARGS__)
#define ERROUT(fmt, ...) Shadow::Logger::debugErrorf(__FILE__, fmt, ##__VA_ARGS__)

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

inline void debugPrintf(std::string caller, const char* fmt, ...) {
#ifdef SHADOW_DEBUG_BUILD
	std::cout << "[" << caller << "] ";
	va_list argList;
	va_start(argList, fmt);
	debugPrintfVargs(fmt, argList);
	va_end(argList);
#endif
}

inline void debugWarnf(std::string caller, const char* fmt, ...) {
#ifdef SHADOW_DEBUG_BUILD
	std::cout << termcolor::yellow << "WARN: {" << caller << "} " << termcolor::reset;
	va_list argList;
	va_start(argList, fmt);
	debugPrintfVargs(fmt, argList);
	va_end(argList);
#endif
}

inline void debugErrorf(std::string caller, const char* fmt, ...) {
#ifdef SHADOW_DEBUG_BUILD
	std::cerr << termcolor::red << "ERROR: {" << caller << "} " << termcolor::reset;
	va_list argList;
	va_start(argList, fmt);
	debugPrintfVargs(fmt, argList);
	va_end(argList);
#endif
}

}

#endif /* SHADOW_NATIVE_DEBUG_LOGGER_HPP */
