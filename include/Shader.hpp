#ifndef SHADOW_NATIVE_SHADER_HPP
#define SHADOW_NATIVE_SHADER_HPP

#include "bgfx/bgfx.h"
#include <string>

namespace Shadow {

bgfx::ShaderHandle loadShader(std::string name);

bgfx::ProgramHandle loadProgram(std::string vsName, std::string fsName);

}

#endif /* SHADOW_NATIVE_SHADER_HPP */