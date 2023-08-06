// Wrapper for json.hpp for calling just json:: and not nlohmann::json::

#ifndef SHADOW_NATIVE_LIB_JSON_IMPL_HPP
#define SHADOW_NATIVE_LIB_JSON_IMPL_HPP

#include <json.hpp>

using json = nlohmann::json;

#endif /* SHADOW_NATIVE_LIB_JSON_IMPL_HPP */