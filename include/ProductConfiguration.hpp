#ifndef SHADOW_NATIVE_PRODUCT_CONFIGURATION_HPP
#define SHADOW_NATIVE_PRODUCT_CONFIGURATION_HPP

#include "DataReader.hpp"
#include <json.hpp>

using json = nlohmann::json;

namespace Shadow {
namespace ProductConfiguration {

	json getConfig(Shadow::DataReader::DataSource::Enum dataSource);

}
}

#endif /* SHADOW_NATIVE_PRODUCT_CONFIGURATION_HPP */