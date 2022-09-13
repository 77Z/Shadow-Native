#ifndef UTIL_H
#define UTIL_H

#include <bgfx/bgfx.h>

#include <bx/bx.h>
#include <bx/file.h>
#include <bx/allocator.h>

bx::AllocatorI* getDefaultAllocator();
bx::AllocatorI* getAllocator();

bx::FileReaderI* getFileReader();
bx::FileWriterI* getFileWriter();

void InitBXFilesystem();
void ShutdownBXFilesytem();

static const bgfx::Memory* loadMem(bx::FileReaderI* reader, const char* filePath);


static bgfx::ShaderHandle loadShader(bx::FileReaderI* reader, const char* name);
static bgfx::ShaderHandle loadShader(const char* name);


bgfx::ProgramHandle loadProgram(bx::FileReaderI* reader, const char* vsName, const char* fsName);
bgfx::ProgramHandle loadProgram(const char* vsName, const char* fsName);

#endif /* UTIL_H */
