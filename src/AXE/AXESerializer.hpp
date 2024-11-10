#ifndef SHADOW_NATIVE_AXE_AXE_SERIALIZER_HPP
#define SHADOW_NATIVE_AXE_AXE_SERIALIZER_HPP

#include "json_impl.hpp"
#include "AXETypes.hpp"

namespace Shadow::AXE {

bool serializeSong(const Song* song, const std::string& filepath);

/// Loads specified BSON encoded file into a Song object
bool deserializeSong(Song* song, const std::string& filepath);

}

#endif /* SHADOW_NATIVE_AXE_AXE_SERIALIZER_HPP */