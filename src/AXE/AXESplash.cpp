#include "Debug/Logger.hpp"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <thread>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "ShadowWindow.hpp"
#include "imgui/theme.hpp" // TODO: Probably uneeded here.
#include "stb_image.h"
#include "bx/bx.h"

namespace Shadow::AXE {

// TODO: Stupid place to put this method
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height) {
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if !BX_PLATFORM_WINDOWS
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
#endif

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}

int showAXESplash() {
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	ShadowWindow window(594, 793, "AXE Audio Workstation", false, true);
	glfwSetWindowSizeLimits(window.window, 594, 793, 594, 793); // GLFW_RESIZABLE doesn't work on undecorated windows

	if (window.window == nullptr) return 1;
	glfwMakeContextCurrent(window.window);
	glfwSwapInterval(1); // Enable vsync

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	ImGui::SetupTheme();

	ImGui_ImplGlfw_InitForOpenGL(window.window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// IMGUI_ENABLE_FREETYPE in imconfig to use Freetype for higher quality font rendering
	float sf = window.getContentScale();
	// float sf = 1.5;
	// TODO: Why does the image only splash screen need font rendering?
	io.Fonts->AddFontFromFileTTF("./Resources/caskaydia-cove-nerd-font-mono.ttf", 16.0f * sf);
	ImGui::GetStyle().ScaleAllSizes(sf);

	int splashWidth = 0;
	int splashHeight = 0;
	GLuint splashTex = 0;
	bool ret = LoadTextureFromFile("./Resources/splash.png", &splashTex, &splashWidth, &splashHeight);
	IM_ASSERT(ret);

	// No while loop because only one frame needs to be rendered at this point.

/* 	while (!window.shouldClose())  */{
		using namespace ImGui;

		window.pollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		NewFrame();

		GetForegroundDrawList()->AddImage((ImTextureID)(intptr_t)splashTex, ImVec2(0, 0), ImVec2(splashWidth, splashHeight));

		Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window.window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());

		glfwSwapBuffers(window.window);
	}

	std::this_thread::sleep_for(std::chrono::seconds(5));

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Bypass ShadowWindow shutdown, preventing GLFW init errors
	glfwDestroyWindow(window.window);

	return 0;
}

}
