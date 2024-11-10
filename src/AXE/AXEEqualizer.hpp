#ifndef SHADOW_NATIVE_AXE_AXE_EQUALIZER_HPP
#define SHADOW_NATIVE_AXE_AXE_EQUALIZER_HPP

namespace Shadow::AXE {

class AXEEqualizer {
public:
	AXEEqualizer();
	~AXEEqualizer();

	void onUpdate(bool& p_open);
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_EQUALIZER_HPP */