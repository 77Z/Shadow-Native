#include "ShadowWindow.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "ShadowWindowUserPointerCarePackage.hpp"
#include "bx/platform.h"
#include <cstring>
#include <unordered_map>
#include <vector>
#include "stb_image.h"
#include "GoDownInFlames.hpp"

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX

#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#include <Windows.h>
#include <dwmapi.h>

#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL

#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
namespace Shadow {

static std::pmr::unordered_map<ShadowEngineCursors_, GLFWcursor*> cursorMap;

ShadowWindow::ShadowWindow(
	int width, int height, std::string title, bool decorations, bool openGlAPI, bool fullscreen)
	: width(width)
	, height(height)
	, windowTitle(title)
	, decorations(decorations)
	, openGlAPI(openGlAPI)
	, fullscreen(fullscreen) {
	initWindow();
}

ShadowWindow::~ShadowWindow() { }

void ShadowWindow::shutdown() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void* ShadowWindow::getNativeWindowHandle() {
#if SHADOW_ENGINE_WAYLAND
	return nullptr;
#elif BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	return (void*)(uintptr_t)glfwGetX11Window(window);
#elif BX_PLATFORM_WINDOWS
	return glfwGetWin32Window(window);
#elif BX_PLATFORM_OSX
	return glfwGetCocoaWindow(window);
#endif
}

void* ShadowWindow::getNativeDisplayHandle() {
#if SHADOW_ENGINE_WAYLAND
	return nullptr;
#elif BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	return glfwGetX11Display();
#else
	return NULL;
#endif
}

float ShadowWindow::getContentScale() {
#ifdef SHADOW_ENGINE_WAYLAND

	return 1.0f;

#endif
	float s;
	glfwGetWindowContentScale(window, &s, nullptr);
	return s;
}

void ShadowWindow::dragWindow() const {
#if BX_PLATFORM_LINUX && !SHADOW_ENGINE_WAYLAND

	const auto display = glfwGetX11Display();

	double x, y;
	int winX, winY;
	glfwGetCursorPos(window, &x, &y);
	glfwGetWindowPos(window, &winX, &winY);

	XClientMessageEvent xclient;
	memset(&xclient, 0, sizeof(XClientMessageEvent));
	XUngrabPointer(display, 0);
	XFlush(display);
	xclient.type = ClientMessage;
	xclient.window = glfwGetX11Window(window);
	xclient.message_type = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
	xclient.format = 32;
	xclient.data.l[0] = winX + x;
	xclient.data.l[1] = winY + y;
	xclient.data.l[2] = 8 /* _NET_WM_MOVERESIZE_MOVE */;
	xclient.data.l[3] = 0;
	xclient.data.l[4] = 0;
	XSendEvent(display, DefaultRootWindow(display), False, SubstructureRedirectMask | SubstructureNotifyMask, (XEvent *)&xclient);
#elif BX_PLATFORM_WINDOWS
#	error Not yet implemented on windows
#endif
}

void ShadowWindow::initWindow() {
	glfwSetErrorCallback(glfw_errorCallback);

	if (!glfwInit())
		BAILOUT("GLFW Failed to Initialize, more info in stdout");
	if (!openGlAPI) {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, (int)decorations);
	glfwWindowHintString(GLFW_WAYLAND_APP_ID, "dev._77z.shadow");

	GLFWmonitor* primary = glfwGetPrimaryMonitor();

	window = glfwCreateWindow(width, height, windowTitle.c_str(), fullscreen ? primary : nullptr, nullptr);

	windowUserPointers["window"] = this;
	glfwSetWindowUserPointer(window, &windowUserPointers);

	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	
	loadCursors();

#ifndef SHADOW_ENGINE_WAYLAND
	{ // Set Window Icon
		int width, height, channels;
		unsigned char* pixels = stbi_load("./Resources/logoScaled.png", &width, &height, &channels, 4);
		if (!pixels) ERROUT("Failed to load window icon");
		else {
			GLFWimage glfwWindowIcon;
			glfwWindowIcon.width = width;
			glfwWindowIcon.height = height;
			glfwWindowIcon.pixels = pixels;
			stbi_image_free(pixels);
			glfwSetWindowIcon(window, 1, &glfwWindowIcon);
		}
	}
#endif

	float windowx, windowy, screenx, screeny = 0;
	glfwGetWindowContentScale(window, &windowx, &windowy);
	glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &screenx, &screeny);

	EC_PRINT("All", "Window content scale: X%.2f Y%.2f", windowx, windowy);
	EC_PRINT("All", "Monitor content scale (associated with window): X%.2f Y%.2f", screenx, screeny);

#if BX_PLATFORM_WINDOWS

	// In this quick and dirty windows port I don't care for drawing
	// my own client titlebar, so this call makes it at least look a
	// little nicer on this ugly operating system.

	HWND hWnd = glfwGetWin32Window(window);

	COLORREF DARK_COLOR = 0x00000000;
	DwmSetWindowAttribute(hWnd, DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR, &DARK_COLOR, sizeof(DARK_COLOR));
	DwmSetWindowAttribute(hWnd, DWMWINDOWATTRIBUTE::DWMWA_BORDER_COLOR, &DARK_COLOR, sizeof(DARK_COLOR));

#endif
}

void ShadowWindow::loadCursors() {

	auto loadInvdividualCursor = [](const char* imgPath, int xhot, int yhot) -> GLFWcursor* {
		int width, height, channels;
		unsigned char* pixels = stbi_load(imgPath, &width, &height, &channels, 4);

		GLFWimage image;

		if (!pixels) {
			ERROUT("Failed to load cursor image %s", imgPath);
			ERROUT("Falling back to white square, some cursors will not be rendered properly!");

			unsigned char data[16 * 16 * 4];
			memset(data, 0xff, sizeof(data));

			image.width  = 16;
			image.height = 16;
			image.pixels = data;
		} else {
			// Successfully loaded

			image.width = width;
			image.height = height;
			image.pixels = pixels;

			stbi_image_free(pixels);
		}

		return glfwCreateCursor(&image, xhot, yhot);
	};

	cursorMap[ShadowEngineCursors_CropClipLeft] = loadInvdividualCursor("./Resources/icons/Cursors/CropClipLeft.png", 16, 11);
	cursorMap[ShadowEngineCursors_CropClipRight] = loadInvdividualCursor("./Resources/icons/Cursors/CropClipRight.png", 0, 0);
	cursorMap[ShadowEngineCursors_ClipSlicing] = loadInvdividualCursor("./Resources/icons/Cursors/ClipSlicing.png", 12, 12);

	if (cursorMap.size() != ShadowEngineCursors_COUNT) {
		ERROUT("Failed to load all cursors, some will not be rendered properly!");
	}

}

void ShadowWindow::setSECursor(ShadowEngineCursors_ cursor) {
	glfwSetCursor(window, cursorMap[cursor]);
}

void ShadowWindow::glfw_errorCallback(int error, const char* description) {
	ERROUT("GLFW error %d: %s\n", error, description);
}

void ShadowWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto userPointers = reinterpret_cast<UserPointersMap*>(glfwGetWindowUserPointer(window));
	ShadowWindow* shadowWindow = static_cast<ShadowWindow*>(userPointers->at("window"));
	shadowWindow->framebufferResized = true;
	shadowWindow->width = width;
	shadowWindow->height = height;
}

}
