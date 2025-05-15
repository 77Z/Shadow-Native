#include "AXE/AXEEditor.hpp"
#include "AXESerializer.hpp"
#include "AXETypes.hpp"
#include "Configuration/EngineConfiguration.hpp"
#include "Debug/Logger.hpp"
#include "ShadowIcons.hpp"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "ShadowWindow.hpp"
#include "imgui/theme.hpp"
#include "GoDownInFlames.hpp"

// Forward declarations
namespace Shadow::AXE::Account {
void onUpdateStatusBar(bool isInEditor, ShadowWindow* window);
}

namespace Shadow::AXE {

Song songToWrite;

static bool openEditorAfterDeath = false;
static std::string projectFileToOpenAfterDeath = "";

static std::vector<std::string> reloadProjects() {
	std::vector<std::string> ret;
	for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(EngineConfiguration::getConfigDir() + "/AXEProjects")) {
		// TODO: Windows only visual-c++ modification??
		std::string name = file.path().filename().string();
		if (name.ends_with(".axe")) {
			ret.push_back(name);
		}
	}
	return ret;
}

int startAXEProjectBrowser(const std::vector<std::string>& args) {

	songToWrite.artist = EngineConfiguration::getUserName();

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

	ShadowWindow window(720, 720, "Shadow Engine - AXE Project Browser", true, true);
	if (window.window == nullptr) return 1;
	glfwMakeContextCurrent(window.window);
	glfwSwapInterval(1); // Enable vsync

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::SetupTheme();

	ImGui_ImplGlfw_InitForOpenGL(window.window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Font loading and scaling
	// IMGUI_ENABLE_FREETYPE in imconfig to use Freetype for higher quality font rendering
	float sf = window.getContentScale();

	float fontSize = 16.0f * sf;
	float iconFontSize = (fontSize * 2.0f / 3.0f) * sf;

	ImVector<ImWchar> ranges;

	ImFontConfig fontCfg;
	fontCfg.OversampleH = 4;
	fontCfg.OversampleV = 4;
	fontCfg.PixelSnapH = false;

	ImFont* primaryFont = io.Fonts->AddFontFromFileTTF("./Resources/Inter-Medium.ttf", fontSize, &fontCfg, ranges.Data);
	// ImFont* primaryFont = io.Fonts->AddFontFromFileTTF("./Resources/arial.ttf", fontSize, &fontCfg, ranges.Data);

	static const ImWchar iconRanges[] = { SHADOW_ICON_MIN, SHADOW_ICON_MAX, 0 };

	ImFontConfig iconFontCfg;
	iconFontCfg.GlyphMinAdvanceX = iconFontSize;
	iconFontCfg.MergeMode = true;
	iconFontCfg.PixelSnapH = true;
	iconFontCfg.OversampleH = 2;
	iconFontCfg.OversampleV = 2;
	iconFontCfg.GlyphOffset.y = 6;
	iconFontCfg.DstFont = primaryFont;

	io.Fonts->AddFontFromFileTTF("./Resources/shadow.ttf", 20.0f * sf, &iconFontCfg, iconRanges);

	ImFont* headingFont = io.Fonts->AddFontFromFileTTF("./Resources/Inter-Black.ttf", 40.0f * sf);

	ImGui::GetStyle().ScaleAllSizes(sf);

	std::vector<std::string> projects = reloadProjects();

	while (!window.shouldClose()) {
		window.pollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// DRAW

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
			| ImGuiWindowFlags_MenuBar;

		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 255));

		ImGui::Begin("RootWindow", nullptr, window_flags);

		ImGui::PopStyleColor();

		ImGui::PopStyleVar(3);

		ImGui::PushFont(headingFont);
		ImGui::Text("    AXE Audio Workstation");
		ImGui::PopFont();

		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - 50, 35));
		Account::onUpdateStatusBar(false, &window);

		if (ImGui::TreeNode("New Song")) {
			ImGui::PushFont(headingFont);
			ImGui::Text("New Song");
			ImGui::Separator();
			ImGui::PopFont();

			ImGui::TextWrapped("Enter some basic information, this can be changed later. If you're "
							   "just experimenting, consider a scheme like Track 1, on the album "
							   "Experiments. It doesn't have to be formal at all");

			ImGui::InputText("Title", &songToWrite.name);
			ImGui::InputText("Artist(s)", &songToWrite.artist);
			ImGui::InputText("Album", &songToWrite.album);
			
			if (ImGui::Button("Create")) {
				std::string loc = EngineConfiguration::getConfigDir() + "/AXEProjects/" + songToWrite.name + ".axe";
				serializeSong(&songToWrite, loc);
				projectFileToOpenAfterDeath = loc;
				openEditorAfterDeath = true;
				window.close();
			}

			ImGui::TreePop();
		}

		ImGui::SeparatorText("Existing Projects");

		ImGui::Indent();
		for (auto& project : projects) {
			if (ImGui::Selectable((std::string(SHADOW_ICON_SYMBOL_METHOD) + " " + project.substr(0, project.size() - 4)).c_str())) {
				projectFileToOpenAfterDeath = EngineConfiguration::getConfigDir() + "/AXEProjects/" + project;
				openEditorAfterDeath = true;
				window.close();
			}
		}
		ImGui::Unindent();

		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window.window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window.window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// We shutdown the ShadowWindow manually to inject openEditorAfterDeath here
	glfwDestroyWindow(window.window);

	// if (openEditorAfterDeath) startAXEEditor(projectFileToOpenAfterDeath);

	glfwTerminate();

	int ret = 0;
	if (openEditorAfterDeath) {
#if BX_PLATFORM_WINDOWS
		// TODO: This needs to properly return a status code for Crashpad to
		// catch anything.
		STARTUPINFOA si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		std::string commandLineStr = args[0] + " axeEditorWithProject \"" + projectFileToOpenAfterDeath + "\"";
		char* commandLine = const_cast<char*>(commandLineStr.c_str());

		if (!CreateProcessA(
				NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi
			)) {

				BAILOUT("I can't open the AXE editor!");
			return -1;
		}

		WaitForSingleObject(pi.hProcess, INFINITE);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
#else
		ret = std::system((args[0] + " axeEditorWithProject \"" + projectFileToOpenAfterDeath + "\"").c_str());
#endif
	}

	return ret;
}

}
