//
// Created by vince on 2/2/26.
//

#ifndef SHADOW_NATIVE_AXE_AXE_OPENGL_UTILS_HPP
#define SHADOW_NATIVE_AXE_AXE_OPENGL_UTILS_HPP

#include <GL/glew.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <string>

namespace Shadow::AXE {

GLuint compileOpenGLShader(GLenum type, const std::string& source);
GLuint OpenGLLoadTextureFromFile(const char* filename, int* out_width, int* out_height);

}

#endif // SHADOW_NATIVE_AXE_AXE_OPENGL_UTILS_HPP
