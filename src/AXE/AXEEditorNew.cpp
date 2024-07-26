#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "AXE/AXEEditor.hpp"
#include "imgui/theme.hpp"
#include "ShadowWindow.hpp"
#include "Debug/EditorConsole.hpp"
#include "AXETypes.hpp"
#include "AXESerializer.hpp"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

namespace Shadow::AXE {

const char* keysPretty[Keys_Count] = {
	"C",
	"C# / D♭",
	"D",
	"D# / E♭",
	"E",
	"F",
	"F# / G♭",
	"G",
	"G# / A♭",
	"A",
	"A# / B♭",
	"B"
};

static Song songInfo;

// Not persistent
struct {

	// Window states
	bool showShadowEngineConsole = false;
	bool showImGuiConsole = false;
	bool showImGuiMetrics = false;
	bool showImGuiStackTool = false;
	bool showClipboardBuffer = false;
} editorState;

int startAXEEditor(AXEProjectEntry project) {
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

	ShadowWindow window(1280, 720, "Shadow Engine - AXE Editor", true, true);
	if (window.window == nullptr) return 1;
	glfwMakeContextCurrent(window.window);
	glfwSwapInterval(1); // Enable vsync

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	ImGui::SetupTheme();

	ImGui_ImplGlfw_InitForOpenGL(window.window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// IMGUI_ENABLE_FREETYPE in imconfig to use Freetype for higher quality font rendering
	float sf = window.getContentScale();
	io.Fonts->AddFontFromFileTTF("./Resources/caskaydia-cove-nerd-font-mono.ttf", 16.0f * sf);
	ImGui::GetStyle().ScaleAllSizes(sf);

	while (!window.shouldClose()) {
		window.pollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
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
				| ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 255));

			ImGui::Begin("RootWindow", nullptr, window_flags);

			ImGui::PopStyleColor();
			ImGui::PopStyleVar(3);

			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("Save Project")) {
						serializeSong(&songInfo);
					}
					if (ImGui::MenuItem("Load")) {
						deserializeSong(&songInfo);
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Exit")) window.close();
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit")) {
					ImGui::MenuItem("Copy", "CTRL + C");
					ImGui::MenuItem("Paste", "CTRL + V");
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Debug Tools")) {
					ImGui::MenuItem("Shadow Engine Debug Console", nullptr, &editorState.showShadowEngineConsole);
					ImGui::MenuItem("UI Console", nullptr, &editorState.showImGuiConsole);
					ImGui::MenuItem("UI Metrics", nullptr, &editorState.showImGuiMetrics);
					ImGui::MenuItem("UI Stack Tool", nullptr, &editorState.showImGuiStackTool);
					ImGui::MenuItem("Clipboard Buffer", nullptr, &editorState.showClipboardBuffer);
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::PushItemWidth(100.0f);
			ImGui::InputFloat("BPM", &songInfo.bpm, 1.0f, 5.0f);
			ImGui::PushItemWidth(120.0f);
			ImGui::SameLine();
			ImGui::InputInt2("Time Signature", songInfo.timeSignature);
			ImGui::SameLine();

			const char* keyName = (songInfo.key >= 0 && songInfo.key < Keys_Count) ? keysPretty[songInfo.key] : "? Unknown ?";
			ImGui::SliderInt("Key", &songInfo.key, 0, Keys_Count - 1, keyName);

			ImGui::SetCursorPosY(55.0f);
			ImGui::DockSpace(ImGui::GetID("AXEDockspace"));
			ImGui::End();
		}

		ImGui::Begin("Timeline");

		ImGui::BeginChild("timelineScroller", ImVec2(ImGui::GetWindowWidth() + 1000.0f, ImGui::GetWindowHeight()), false, ImGuiWindowFlags_AlwaysHorizontalScrollbar);

		int trackIt = 0;
		for (auto& track : songInfo.tracks) {
			ImGui::PushID(trackIt);

			ImGui::Text("%s", track.name.c_str());
			ImGui::InputText("##trackname", &track.name);
			
			ImGui::PopID();
			trackIt++;
		}

		ImGui::EndChild();

		ImGui::End();

		if (editorState.showShadowEngineConsole) EditorConsole::Frontend::onUpdate();
		if (editorState.showImGuiConsole) ImGui::ShowDebugLogWindow(&editorState.showImGuiConsole);
		if (editorState.showImGuiMetrics) ImGui::ShowMetricsWindow(&editorState.showImGuiMetrics);
		if (editorState.showImGuiStackTool) ImGui::ShowStackToolWindow(&editorState.showImGuiStackTool);

		ImGui::ShowDemoWindow();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window.window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window.window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	window.shutdown();

	return 0;
}

}