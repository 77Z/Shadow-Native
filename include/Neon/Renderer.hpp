#ifndef SHADOW_NATIVE_NEON_RENDERER_HPP
#define SHADOW_NATIVE_NEON_RENDERER_HPP

#include "bx/math.h"

namespace Shadow::Neon {
	
class NeonRenderer {
public:
	void setViewMtx(const bx::Vec3& eyc, const bx::Vec3& at, const bx::Vec3& up = { 0, 1, 0 });
	void setProjMtx(float fov, float aspectRatio, float near, float far);
	void setViewport(float x, float y, float width, float height);
	void prepare();
	void add(Neon::Mesh)
};

}

#endif /* SHADOW_NATIVE_NEON_RENDERER_HPP */