#ifndef LOGGER_H
#define LOGGER_H

// General purpose logging tool for Shadow Engine
// to make sure that general strings aren't shown
// in production and to show the original caller.

#include <iostream>
#include <string>
#include <termcolor.hpp>

#define PRINT(msg) Shadow::Logger::internalPrint(__FILE__, msg)
#define WARN(msg) Shadow::Logger::internalWarn(__FILE__, msg)
#define ERROUT(msg) Shadow::Logger::internalPrintError(__FILE__, msg)

//TODO: add string formatting like stdio.h printf
//EX: (std::string caller, std::string fmt, ...)
//internalPrint("caller", "Cool number: %i", number);
namespace Shadow {
	namespace Logger {
		inline void internalPrint(std::string caller, std::string message) {
#ifdef SHADOW_DEBUG_BUILD
			std::cout << "[" << caller << "] " << message << std::endl;
#endif
		}

		inline void internalWarn(std::string caller, std::string message) {
#ifdef SHADOW_DEBUG_BUILD
			std::cout <<
				termcolor::yellow		<<
				"WARN: {" << caller << "} "	<<
				termcolor::reset		<<
				message				<<
				std::endl;
#endif
		}

		inline void internalPrintError(std::string caller, std::string errmsg) {
#ifdef SHADOW_DEBUG_BUILD
			std::cerr <<
				termcolor::red			<<
				"ERROR: {" << caller << "} "	<<
				termcolor::reset		<<
				errmsg				<<
				std::endl;
#endif
		}
	}
}

#endif /* LOGGER_H */
