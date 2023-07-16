#include "ProductConfiguration.hpp"
#include "DataReader.hpp"
#include "Debug/Logger.h"
#include <fstream>
#include <json.hpp>

using json = nlohmann::json;

namespace Shadow {

json ProductConfiguration::getConfig(Shadow::DataReader::DataSource::Enum dataSource) {
	std::ifstream file("./product.chunk/product.json");
	if (!file)
		ERROUT("Product configuration not found");
	json data = json::parse(file);

	return data;
}

}