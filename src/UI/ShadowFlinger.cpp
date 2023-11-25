#include "UI/ShadowFlinger.hpp"
#include "Debug/Logger.hpp"
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

	struct PosColorVertex
	{
		float x;
		float y;
		float z;
		uint32_t abgr;
	};

	static PosColorVertex cubeVertices[] =
	{
		{-1.0f,  1.0f,  1.0f, 0xff000000 },
		{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
		{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
		{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
		{-1.0f,  1.0f, -1.0f, 0xffff0000 },
		{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
		{-1.0f, -1.0f, -1.0f, 0xffffff00 },
		{ 1.0f, -1.0f, -1.0f, 0xffffffff },
	};

	static const uint16_t cubeTriList[] =
	{
		0, 1, 2,
		1, 3, 2,
		4, 6, 5,
		5, 6, 7,
		0, 2, 4,
		4, 2, 6,
		1, 5, 3,
		5, 7, 3,
		0, 4, 1,
		4, 5, 1,
		2, 3, 6,
		6, 3, 7,
	};

	const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
	const bx::Vec3 eye = { 0.0f, 0.0f, -5.0f };

	ShadowFlingerViewport::ShadowFlingerViewport(bgfx::ViewId viewId)
		: viewId(viewId) {
		// drawDecl.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).end();

		// vbh = bgfx::createVertexBuffer(bgfx::makeRef(rectangle, sizeof(rectangle)), drawDecl);
		// ibh = bgfx::createIndexBuffer(bgfx::makeRef(rectangleTriList, sizeof(rectangleTriList)));


		drawDecl.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();

		vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), drawDecl);
		ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));
	}

	void ShadowFlingerViewport::resized() {
		bgfx::setViewRect(viewId, 0, 0, bgfx::BackbufferRatio::Equal);
	}

	void ShadowFlingerViewport::draw(bgfx::ProgramHandle program, int width, int height) {
		// bgfx::setViewRect(viewId, 0, 0, bgfx::BackbufferRatio::Equal);

		float view[16];
		bx::mtxLookAt(view, eye, at);
		float projectionMatrix[16];
		bx::mtxProj(projectionMatrix, 60.0f, float(width) / float(height), 0.1f, 100.0f,
			bgfx::getCaps()->homogeneousDepth);

		// float ortho[16];
		// bx::mtxOrtho(ortho, 0.0f, width, height, 0.0f, 0.0f, 1000.0f, 0.0f,
		// 	bgfx::getCaps()->homogeneousDepth);

		// bgfx::setViewTransform(viewId, view, ortho);
		bgfx::setViewTransform(viewId, view, projectionMatrix);
		bgfx::setViewRect(viewId, 0, 0, uint16_t(width), uint16_t(height));

		bgfx::touch(viewId);

		PRINT("Draw");

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