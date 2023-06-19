#ifndef UTIL_H
#define UTIL_H

#include <bgfx/bgfx.h>

#include <bx/bx.h>
#include <bx/file.h>
#include <bx/allocator.h>
#include <cstdint>

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


namespace Utilities {

	struct MouseButton {
		enum Enum {
			None,
			Left,
			Middle,
			Right,

			Count
		};
	};
	
	struct MouseState {
		MouseState() : m_mx(0), m_my(0), m_mz(0) {
			for (uint32_t i = 0; i < Utilities::MouseButton::Count; i++) {
				m_buttons[i] = Utilities::MouseButton::None;
			}
		}

		int32_t m_mx;
		int32_t m_my;
		int32_t m_mz;
		uint8_t m_buttons[Utilities::MouseButton::Count];
	};

}

#endif /* UTIL_H */
