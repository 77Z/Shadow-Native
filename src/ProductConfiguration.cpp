#include "ProductConfiguration.hpp"
#include "DataReader.hpp"
#include "Debug/Logger.hpp"
#include <fstream>
#include <json_impl.hpp>

namespace Shadow {

json ProductConfiguration::getConfig(Shadow::DataReader::DataSource::Enum dataSource) {
	std::ifstream file("./Resources/product.chunk/product.json");
	if (!file)
		ERROUT("Product configuration not found");
	json data = json::parse(file);

	return data;
}

}