#ifndef UTIL_H
#define UTIL_H

#include <bx/bx.h>
#include <bx/file.h>
#include <bx/allocator.h>

bx::AllocatorI* getDefaultAllocator();
bx::AllocatorI* getAllocator();

bx::FileReaderI* getFileReader();
bx::FileWriterI* getFileWriter();

void InitBXFilesystem();
void ShutdownBXFilesytem();

#endif /* UTIL_H */
