#include "Shader.hpp"

namespace Shadow {

bgfx::ShaderHandle loadShader(std::string name) {
#if SHADOW_SHADER_EMBEDDED

#else

#endif
}

bgfx::ProgramHandle loadProgram(std::string vsName, std::string fsName) { }

}