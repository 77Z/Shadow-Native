#include <Util.h>

#include "Logger.h"
#include "bx/readerwriter.h"

#include <bgfx/bgfx.h>

#include <bx/allocator.h>
#include <bx/bx.h>
#include <bx/file.h>

#include "../3rdparty/meshoptimizer/src/meshoptimizer.h"

#include <bimg/bimg.h>
#include <bimg/decode.h>

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
	bx::deleteObject(g_allocator, s_fileReader);
	bx::deleteObject(g_allocator, s_fileWriter);
	s_fileReader = nullptr;
	s_fileWriter = nullptr;
}

void* load(
	bx::FileReaderI* reader, bx::AllocatorI* _allocator, const char* _filePath, uint32_t* _size) {
	if (bx::open(reader, _filePath)) {
		uint32_t size = (uint32_t)bx::getSize(reader);
		void* data = bx::alloc(_allocator, size);
		bx::read(reader, data, size, bx::ErrorAssert {});
		bx::close(reader);
		if (NULL != _size)
			*_size = size;
		return data;
	} else {
		ERROUT("Failed to open: %s.", _filePath);
	}

	if (NULL != _size) {
		*_size = 0;
	}

	return NULL;
}

void* load(const char* _filePath, uint32_t* _size) {
	return load(getFileReader(), getAllocator(), _filePath, _size);
}

void unload(void* _ptr) { bx::free(getAllocator(), _ptr); }

static const bgfx::Memory* loadMem(bx::FileReaderI* reader, const char* filePath) {
	if (bx::open(reader, filePath)) {
		uint32_t size = (uint32_t)bx::getSize(reader);
		const bgfx::Memory* mem = bgfx::alloc(size + 1);
		bx::read(reader, mem->data, size, bx::ErrorAssert {});
		bx::close(reader);
		mem->data[mem->size - 1] = '\0';
		return mem;
	}

	ERROUT("Failed to load %s", filePath);
	return NULL;
}

static bgfx::ShaderHandle loadShader(bx::FileReaderI* reader, const char* name) {
	char filePath[512];

	const char* shaderPath = "../res/shaders/";

	bx::strCopy(filePath, BX_COUNTOF(filePath), shaderPath);
	bx::strCat(filePath, BX_COUNTOF(filePath), name);
	bx::strCat(filePath, BX_COUNTOF(filePath), ".bin");

	bgfx::ShaderHandle handle = bgfx::createShader(loadMem(reader, filePath));
	bgfx::setName(handle, name);

	return handle;
}

static bgfx::ShaderHandle loadShader(const char* name) { return loadShader(getFileReader(), name); }

// Nice function to load both shaders and return a program automatically
bgfx::ProgramHandle loadProgram(bx::FileReaderI* reader, const char* vsName, const char* fsName) {
	bgfx::ShaderHandle vsh = loadShader(reader, vsName);
	bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
	if (nullptr != fsName)
		fsh = loadShader(reader, fsName);

	return bgfx::createProgram(vsh, fsh, true);
}

bgfx::ProgramHandle loadProgram(const char* vsName, const char* fsName) {
	return loadProgram(getFileReader(), vsName, fsName);
}

static void imageReleaseCb(void* _ptr, void* _userData) {
	BX_UNUSED(_ptr);
	bimg::ImageContainer* imageContainer = (bimg::ImageContainer*)_userData;
	bimg::imageFree(imageContainer);
}

/*bgfx::TextureHandle loadTexture(bx::FileReaderI* reader, const char* _filePath, uint64_t _flags,
uint8_t _skip, bgfx::TextureInfo* _info, bimg::Orientation::Enum* _orientation) { BX_UNUSED(_skip);
	bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;

	uint32_t size;
	void* data = load(reader, getAllocator(), _filePath, &size);
	if (NULL != data) {
		bimg::ImageContainer* imageContainer = bimg::imageParse(getAllocator(), data, size);

		if (NULL != imageContainer) {
			if (NULL != _orientation)
				*_orientation = imageContainer->m_orientation;

			const bgfx::Memory* mem = bgfx::makeRef(
					  imageContainer->m_data
					, imageContainer->m_size
					, imageReleaseCb
					, imageContainer
					);
			unload(data);

			if (imageContainer->m_cubeMap) {
				handle = bgfx::createTextureCube(
					  uint16_t(imageContainer->m_width)
					, 1 < imageContainer->m_numMips
					, imageContainer->m_numLayers
					, bgfx::TextureFormat::Enum(imageContainer->m_format)
					, _flags
					, mem
					);
			} else if (1 < imageContainer->m_depth) {
				handle = bgfx::createTexture3D(
					  uint16_t(imageContainer->m_width)
					, uint16_t(imageContainer->m_height)
					, uint16_t(imageContainer->m_depth)
					, 1 < imageContainer->m_numMips
					, bgfx::TextureFormat::Enum(imageContainer->m_format)
					, _flags
					, mem
					);
			}
			else if (bgfx::isTextureValid(0, false, imageContainer->m_numLayers,
bgfx::TextureFormat::Enum(imageContainer->m_format), _flags)) { handle = bgfx::createTexture2D(
					  uint16_t(imageContainer->m_width)
					, uint16_t(imageContainer->m_height)
					, 1 < imageContainer->m_numMips
					, imageContainer->m_numLayers
					, bgfx::TextureFormat::Enum(imageContainer->m_format)
					, _flags
					, mem
					);
			}

			if (bgfx::isValid(handle))
				bgfx::setName(handle, _filePath);

			if (NULL != _info) {
				bgfx::calcTextureSize(
					  *_info
					, uint16_t(imageContainer->m_width)
					, uint16_t(imageContainer->m_height)
					, uint16_t(imageContainer->m_depth)
					, imageContainer->m_cubeMap
					, 1 < imageContainer->m_numMips
					, imageContainer->m_numLayers
					, bgfx::TextureFormat::Enum(imageContainer->m_format)
					);
			}
		}
	}

	return handle;
}

bgfx::TextureHandle loadTexture(const char* _name, uint64_t _flags, uint8_t _skip,
bgfx::TextureInfo* _info, bimg::Orientation::Enum* _orientation) { return
loadTexture(getFileReader(), _name, _flags, _skip, _info, _orientation);
}*/
