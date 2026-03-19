//
// Created by vince on 2/2/26.
//

#include "AXEOpenGLUtils.hpp"

#include "stb_image.h"

namespace Shadow::AXE {

GLuint compileOpenGLShader(GLenum type, const std::string& source) {
	GLuint shader = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr); // OpenGL expects the source as a pointer to a char pointer
	glCompileShader(shader);

	// TODO: Add error checking using glGetShaderiv and glGetShaderInfoLog

	return shader;
}

GLuint OpenGLLoadTextureFromFile(const char* filename, int* out_width, int* out_height) {
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, nullptr, 4);
	if (image_data == nullptr)
		return 0;

	// Create OpenGL texture
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on some OpenGL drivers
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // This is required on some OpenGL drivers

	// Upload pixels into texture
	// Use GL_RGBA for 4 channels
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_width = image_width;
	*out_height = image_height;

	return image_texture;
}

}