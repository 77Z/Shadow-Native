#ifndef SHADOW_NATIVE_MODEL_VIEWER_HPP
#define SHADOW_NATIVE_MODEL_VIEWER_HPP

#include <string>

namespace Shadow {

class ModelViewer {
public:
	ModelViewer(const std::string& filePath);
	~ModelViewer();

private:
	bool wireframe = false;
};

}

#endif /* SHADOW_NATIVE_MODEL_VIEWER_HPP */