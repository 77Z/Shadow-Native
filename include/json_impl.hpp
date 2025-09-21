// Use this wrapper for json.hpp

#ifndef SHADOW_NATIVE_JSON_IMPL_HPP
#define SHADOW_NATIVE_JSON_IMPL_HPP

#include <json.hpp>
#include <string>

using json = nlohmann::json;

namespace Shadow::JSON {

void dumpJsonToBson(json& obj, const std::string& filepath);

void dumpJsonToFile(json& obj, const std::string& filepath, bool formatted = false);

json readBsonFile(std::string filepath);

json readJsonFile(std::string filepath);

}

#endif /* SHADOW_NATIVE_JSON_IMPL_HPP */