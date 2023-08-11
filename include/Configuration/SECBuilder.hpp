#ifndef SHADOW_NATIVE_CONFIGURATION_SEC_BUILDER_HPP
#define SHADOW_NATIVE_CONFIGURATION_SEC_BUILDER_HPP

#include <string>

namespace Shadow::Configuration {

class SECBuilder {
public:
	SECBuilder(std::string fileName);
	~SECBuilder();

	void add(std::string key, std::string value);
	void write();

private:
	std::string fileName;
	std::string out;
	bool closed = false;
};

}

#endif /* SHADOW_NATIVE_CONFIGURATION_SEC_BUILDER_HPP */