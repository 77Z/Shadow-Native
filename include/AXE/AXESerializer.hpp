#ifndef SHADOW_NATIVE_AXE_AXE_SERIALIZER_HPP
#define SHADOW_NATIVE_AXE_AXE_SERIALIZER_HPP

#include "json_impl.hpp"

namespace Shadow::AXE {

class AXESerializer {
public:
	AXESerializer();
	~AXESerializer();

private:
	json outjson;

	void serializeTrack();
	void serializeClip();
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_SERIALIZER_HPP */