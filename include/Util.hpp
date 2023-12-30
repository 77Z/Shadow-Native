#ifndef SHADOW_NATIVE_UTIL_HPP
#define SHADOW_NATIVE_UTIL_HPP

#include "bimg/bimg.h"
#include <bgfx/bgfx.h>

#include <bx/allocator.h>
#include <bx/bx.h>
#include <bx/file.h>
#include <cstdint>
#include <string>
#include <vector>

bx::AllocatorI* getDefaultAllocator();
bx::AllocatorI* getAllocator();

bx::FileReaderI* getFileReader();
bx::FileWriterI* getFileWriter();

void InitBXFilesystem();
void ShutdownBXFilesytem();

// TODO: New API for all of this in the future?
const bgfx::Memory* loadMem(bx::FileReaderI* reader, const char* filePath);

bgfx::ShaderHandle loadShader(bx::FileReaderI* reader, const char* name);
bgfx::ShaderHandle loadShader(const char* name);

bgfx::ProgramHandle loadProgram(bx::FileReaderI* reader, const char* vsName, const char* fsName);
bgfx::ProgramHandle loadProgram(const char* vsName, const char* fsName);

void imageReleaseCb(void* _ptr, void* _userData);
bgfx::TextureHandle loadTexture(const char* _name,
	uint64_t _flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, uint8_t _skip = 0,
	bgfx::TextureInfo* _info = NULL, bimg::Orientation::Enum* _orientation = NULL);

namespace Shadow::Util {

std::string removeDupeSlashes(std::string input);
std::vector<std::string> splitString(const std::string& str, char delimeter);

void openURL(const std::string &url);

}

#endif /* SHADOW_NATIVE_UTIL_HPP */