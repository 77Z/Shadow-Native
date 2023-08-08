// Use this wrapper for json.hpp

#ifndef SHADOW_NATIVE_JSON_IMPL_HPP
#define SHADOW_NATIVE_JSON_IMPL_HPP

#include <json.hpp>
#include <string>

using json = nlohmann::json;

namespace Shadow::JSON {

void dumpJsonToBson(json obj, std::string filepath);

json readBsonFile(std::string filepath);

}

#endif /* SHADOW_NATIVE_JSON_IMPL_HPP */