#include "Analytics.hpp"
#include "Debug/Logger.hpp"
#include "Util.hpp"
#include "bgfx/defines.h"
#include "bx/bx.h"
#include "../lib/bgfx/src/version.h"
#include "json_impl.hpp"
#include "generated/autoconf.h"
#include <fstream>
#include <ios>
#include <sstream>
#include <vector>

namespace Shadow {

/// Returns "FAILED TO READ" on failure
static std::string quickReadFile(const std::string& filePath) {
	std::ifstream file(filePath);

	if (!file) return "FAILED TO READ";

	std::stringstream ss;
	ss << file.rdbuf();
	std::string output = ss.str();
	output.pop_back();
	return output;
}

/// Returns "FAILED TO READ" on failure
static std::string quickReadCommand(const char* cmd) {
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	
	if (!pipe) return "FAILED TO READ";

	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}

/// ! ANALYTICS SHOULD RUN ON ITS OWN THREAD
AnalyticsBroker::AnalyticsBroker() {
	json trackingObj;
	trackingObj["CPU_NAME"] = BX_CPU_NAME;
	trackingObj["CPU_ARCH"] = BX_ARCH_NAME;
	trackingObj["PLATFORM_NAME"] = BX_PLATFORM_NAME;
	trackingObj["CRT"] = BX_CRT_NAME;
	trackingObj["COMPILER"] = BX_COMPILER_NAME;
	trackingObj["BGFX_API_VERSION"] = BGFX_API_VERSION;
	trackingObj["BGFX_REV"] = BGFX_REV_NUMBER;
	trackingObj["BGFX_HASH"] = BGFX_REV_SHA1;
	trackingObj["SHADOW_VERSION"] = SHADOW_VERSION_STRING;
	trackingObj["HORDE_VERSION"] = HORDE_VERSION_STRING;
	trackingObj["SHADOW_HASH"] = SHADOW_COMMIT_HASH;
	//TODO: Hardware names??? lscpu, gpu used for graphics, etc.
	
	std::ifstream cpuinfoFile("/proc/cpuinfo");
	if (cpuinfoFile) {
		std::stringstream cpuName;
		char ch;
		int i = 0;
		while (cpuinfoFile >> std::noskipws >> ch) {
			if (ch == ':') {
				i++;
				continue;
			}
			if (i == 5) {
				if (ch == '\n') break;
				cpuName << ch;
			}
		}

		cpuinfoFile.close();

		trackingObj["CPU_NAME"] = cpuName.str();
	} else {
		trackingObj["CPU_NAME"] = "FAILED TO READ";
	}

	trackingObj["HARDWARE_BIOS_VENDOR"] = quickReadFile("/sys/devices/virtual/dmi/id/bios_vendor");
	trackingObj["HARDWARE_BIOS_VERSION"] = quickReadFile("/sys/devices/virtual/dmi/id/bios_version");

	trackingObj["HARDWARE_BOARD_VENDOR"] = quickReadFile("/sys/devices/virtual/dmi/id/board_vendor");
	trackingObj["HARDWARE_PRODUCT_NAME"] = quickReadFile("/sys/devices/virtual/dmi/id/product_name");


	trackingObj["HARDWARE_PCI_LIST"] = Util::splitString(quickReadCommand("lspci"), '\n');
	trackingObj["HARDWARE_USB_LIST"] = Util::splitString(quickReadCommand("lsusb"), '\n');

	PRINT("%s", trackingObj.dump(2).c_str());

}

AnalyticsBroker::~AnalyticsBroker() {}

}