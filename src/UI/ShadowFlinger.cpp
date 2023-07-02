#include "UI/ShadowFlinger.hpp"
#include "bgfx/bgfx.h"
#include <bx/math.h>

namespace Shadow {
namespace UI {

	const UIVertex rectangle[] = {
		{ -1000.0f, -1000.0f, 0.0f },
		{ 1000.0f, -1000.0f, 0.0f },
		{ -1000.0f, 1000.0f, 0.0f },
		{ 1000.0f, 1000.0f, 0.0f },
	};

	const u16 rectangleTriList[] = {
		0,
		1,
		2,
		1,
		3,
		2,
	};

	const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
	const bx::Vec3 eye = { 0.0f, 0.0f, -1.0f };

	ShadowFlingerViewport::ShadowFlingerViewport(bgfx::ViewId viewId)
		: viewId(viewId) {
		drawDecl.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).end();

		vbh = bgfx::createVertexBuffer(bgfx::makeRef(rectangle, sizeof(rectangle)), drawDecl);
		ibh = bgfx::createIndexBuffer(bgfx::makeRef(rectangleTriList, sizeof(rectangleTriList)));
	}

	void ShadowFlingerViewport::resized() {
		bgfx::setViewRect(viewId, 0, 0, bgfx::BackbufferRatio::Equal);
	}

	void ShadowFlingerViewport::draw(bgfx::ProgramHandle program, int width, int height) {
		// bgfx::setViewRect(viewId, 0, 0, bgfx::BackbufferRatio::Equal);

		float view[16];
		bx::mtxLookAt(view, eye, at);
		// float projectionMatrix[16];
		// bx::mtxProj(projectionMatrix, 60.0f, float(width) / float(height), 0.1f, 100.0f,
		// 	bgfx::getCaps()->homogeneousDepth);

		float ortho[16];
		bx::mtxOrtho(ortho, 0.0f, width, height, 0.0f, 0.0f, 1000.0f, 0.0f,
			bgfx::getCaps()->homogeneousDepth);

		bgfx::setViewTransform(viewId, view, ortho);
		bgfx::setViewRect(viewId, 0, 0, uint16_t(width), uint16_t(height));

		bgfx::touch(viewId);

		bgfx::setVertexBuffer(0, vbh);
		bgfx::setIndexBuffer(ibh);

		bgfx::submit(viewId, program);
	}

	void ShadowFlingerViewport::unload() {
		bgfx::destroy(vbh);
		bgfx::destroy(ibh);
	}

}
}