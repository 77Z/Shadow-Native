#include "ProductionRuntime.hpp"
#include "Debug/Logger.h"
#include "ProductConfiguration.hpp"

#include <iostream>
#include <json_impl.hpp>
#include <sstream>
#include <string>

namespace Shadow {

int StartProductionRuntime() {

	auto productConfiguration = ProductConfiguration::getConfig(DataReader::DataSource::Filesystem);

	std::stringstream ss;
	ss << productConfiguration["ProductName"];

	PRINT("%s", ss.str().c_str());

	return 0;
}

}