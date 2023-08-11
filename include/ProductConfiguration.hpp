#ifndef SHADOW_NATIVE_PRODUCT_CONFIGURATION_HPP
#define SHADOW_NATIVE_PRODUCT_CONFIGURATION_HPP

#include "DataReader.hpp"
#include <json_impl.hpp>

namespace Shadow::ProductConfiguration {

json getConfig(Shadow::DataReader::DataSource::Enum dataSource);

}

#endif /* SHADOW_NATIVE_PRODUCT_CONFIGURATION_HPP */