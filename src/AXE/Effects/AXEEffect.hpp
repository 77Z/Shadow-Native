#ifndef SHADOW_NATIVE_AXE_EFFECTS_AXE_EFFECT_HPP
#define SHADOW_NATIVE_AXE_EFFECTS_AXE_EFFECT_HPP

#include <string>

class AXEEffect {
public:
	virtual ~AXEEffect() = default;
	virtual void apply() = 0;
	virtual std::string getName() const = 0;
};

#endif /* SHADOW_NATIVE_AXE_EFFECTS_AXE_EFFECT_HPP */