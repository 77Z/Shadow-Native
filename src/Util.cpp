#include <Util.h>

#include <bx/bx.h>
#include <bx/file.h>
#include <bx/allocator.h>

bx::AllocatorI* getDefaultAllocator() {
	static bx::DefaultAllocator s_allocator;
	return &s_allocator;
}

extern bx::AllocatorI* getDefaultAllocator();
bx::AllocatorI* g_allocator = getDefaultAllocator();

bx::AllocatorI* getAllocator() {
	if (g_allocator == nullptr)
		g_allocator = getDefaultAllocator();

	return g_allocator;
}


static bx::FileReaderI* s_fileReader = nullptr;
static bx::FileWriterI* s_fileWriter = nullptr;

bx::FileReaderI* getFileReader() { return s_fileReader; }
bx::FileWriterI* getFileWriter() { return s_fileWriter; }

void InitBXFilesystem() {
	s_fileReader = BX_NEW(g_allocator, bx::FileReader);
	s_fileWriter = BX_NEW(g_allocator, bx::FileWriter);
}

void ShutdownBXFilesytem() {
	BX_DELETE(g_allocator, s_fileReader);
	BX_DELETE(g_allocator, s_fileWriter);
	s_fileReader = nullptr;
	s_fileWriter = nullptr;
}

