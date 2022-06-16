//
// Created by Vince on 6/16/22.
//

#ifndef SHADOW_NATIVE_BGFX_UTILS_H
#define SHADOW_NATIVE_BGFX_UTILS_H

#include <bgfx/bgfx.h>

#include <bx/readerwriter.h>

struct Mesh {
	void load(bx::ReaderSeekerI* reader, bool ramcopy);
	bgfx::VertexLayout m_layout;
};

#endif /* SHADOW_NATIVE_BGFX_UTILS_H */
