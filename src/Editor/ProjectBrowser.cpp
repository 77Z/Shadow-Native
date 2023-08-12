#include "Editor/ProjectBrowser.hpp"
#include "Configuration/EngineConfiguration.hpp"
#include "Configuration/SECBuilder.hpp"
#include "Core.hpp"
#include "Debug/Logger.hpp"
#include "Debug/Profiler.hpp"
#include "Editor/Editor.hpp"
#include "Runtime.hpp"
#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneSerializer.hpp"
#include "ShadowWindow.hpp"
#include "Util.hpp"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bx/platform.h"
#include "bx/timer.h"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "imgui/theme.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_internal.h"
#include "json_impl.hpp"
#include <GLFW/glfw3.h>
#include <cstring>
#include <filesystem>
#include <imgui/imgui_impl_bgfx.h>
#include <regex>
#include <string>
#include <vector>

using ProjectEntry = Shadow::Editor::ProjectEntry;

static bool vsync = true;
static bool openEditorAfterDeath = false;
static bool openLegacyEditorAfterDeath = false;

std::vector<ProjectEntry> gblProjects;

static Shadow::ShadowWindow* refWindow;

static ImFont* mainFont;
static ImFont* headingFont;

static std::string projectName = "My Creation";
static std::string packageID = "com.example.mycreation";
// static std::string projectLocation = "~/Desktop/ShadowProjects";

static std::string issue = "";

static ProjectEntry editorNowProject;
static void openEditorNow(ProjectEntry project) {
	editorNowProject = project;
	openEditorAfterDeath = true;
	refWindow->close();
}

static std::string projectNameChecker(std::string inp) {

#if __cplusplus > BX_LANGUAGE_CPP17
	if (inp.empty())
		return "Name can't be blank";
	if (inp.ends_with("."))
		return "Windows doesn't allow '.' ending";
	if (inp.ends_with(" "))
		return "Windows doesn't allow ' ' ending";
	if (inp.starts_with("."))
		return "Don't start with a '.'";
	if (inp.starts_with(" "))
		return "Don't start with a ' '";
	if (inp == "CON")
		return "Windows doesn't allow this name";
	if (inp == "PRN")
		return "Windows doesn't allow this name";
	if (inp == "AUX")
		return "Windows doesn't allow this name";
	if (inp == "NUL")
		return "Windows doesn't allow this name";
	if (inp == "COM")
		return "Windows doesn't allow this name";
	if (inp == "LPT")
		return "Windows doesn't allow this name";

	// TODO: This does not match lol
	std::regex pattern("[/\\<>:\"|?* ]");
	if (std::regex_match(inp, pattern))
		return "Don't include any of the following characters: / \\ < > : \" | ? * space";

	return "";
#else
	return "";
#endif
}

static int projectNameEditCallback(ImGuiInputTextCallbackData* data) {
	if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
		issue = projectNameChecker((std::string)data->Buf);
	}
	return 0;
}

static void createProject() {
	PRINT("Making new project %s", projectName.c_str());

	const std::string projDir
		= Shadow::EngineConfiguration::getConfigDir() + "/Projects/" + projectName;

	if (std::filesystem::exists(projDir)) {
		ERROUT("Project already exists! [%s]", projectName.c_str());
		return;
	}

	std::filesystem::create_directory(projDir);
	std::filesystem::create_directory(projDir + "/Content");

	json projectCfg = {
		{ "name", projectName },
		{ "pkgId", packageID },
		{ "engineVer", "0.1.0" },
		{ "default-scene", "default.scene" },
	};

	Shadow::JSON::dumpJsonToBson(projectCfg, projDir + "/project.sec");

	Shadow::Reference<Shadow::Scene> defaultScene
		= Shadow::CreateReference<Shadow::Scene>("Default Scene");
	Shadow::SceneSerializer ss(defaultScene);

	Shadow::Entity floor = defaultScene->createEntity("Floor");
	auto& t = floor.addComponent<Shadow::TransformComponent>();
	t.scale.x = 5.0f;
	t.scale.y = 0.2f;
	t.scale.z = 5.0f;
	t.translation.y = -1.5f;
	floor.addComponent<Shadow::CubeComponent>(1.0f);

	ss.serialize(projDir + "/Content/default.scene");

#if 0
	Shadow::Configuration::SECBuilder projSec(projDir + "/project.sec");
	projSec.add("name", projectName);
	projSec.add("pkgId", packageID);
	projSec.add("engineVer", "0.1.0");
	projSec.add("default-scene", "default.scene");
	projSec.write();
#endif

	// openEditorNow(projDir);
}

static bool projectNode(ProjectEntry project) {
	const char* label = project.name.c_str();
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size(100, 130);
	ImRect boundingBox(pos, ImVec2(pos.x + size.x, pos.y + size.y));

	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImGui::ItemSize(size);
	if (!ImGui::ItemAdd(boundingBox, id))
		return false;

	// Render
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(boundingBox, id, &hovered, &held);

	if (hovered && ImGui::BeginTooltip()) {
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(56, 76, 255, 255));
		ImGui::TextUnformatted(label);
		ImGui::PopStyleColor();
		ImGui::Separator();
		ImGui::Text("%s", project.path.c_str());
		ImGui::EndTooltip();
	}

	const ImU32 color
		= ImGui::GetColorU32(hovered ? IM_COL32(34, 34, 34, 255) : IM_COL32(20, 20, 20, 255));
	ImGui::RenderNavHighlight(boundingBox, id);
	ImGui::RenderFrame(boundingBox.Min, boundingBox.Max, color, true, 3.0f);

	ImGui::RenderTextClipped(ImVec2(boundingBox.Min.x, boundingBox.Min.y + 100),
		ImVec2(boundingBox.Max.x, boundingBox.Max.y + 100), label, NULL, &label_size, ImVec2(0, 0),
		&boundingBox);

	window->DrawList->AddImage(ImGui::toId(project.icon, 0, 0), boundingBox.Min,
		ImVec2(boundingBox.Max.x, boundingBox.Max.y - 30));

	// GetForegroundDrawList()->AddRect(
	// 	boundingBox.Min, boundingBox.Max, IM_COL32(255, 0, 0, 255));

	if (pressed) {
		openEditorNow(project);
	}

	ImGui::SameLine();
	return pressed;
}

static std::vector<ProjectEntry> getProjects() {
	std::vector<ProjectEntry> projects;
	std::string projectsDir = Shadow::EngineConfiguration::getConfigDir() + "/Projects";

	for (const auto& file : std::filesystem::directory_iterator(projectsDir)) {
		if (!file.is_directory())
			continue;
		ProjectEntry project;
		project.path = file.path().string();
		project.name
			= project.path.substr(project.path.find_last_of("/") + 1, project.path.length());
		std::string iconPath = project.path + "/icon.png";
		PRINT("%s", iconPath.c_str());
		project.icon = loadTexture(iconPath.c_str());

		projects.push_back(project);
	}

	return projects;
}

static void drawProjectBrowser() {
	ImGui::Begin("Project Browser");

	ImGui::PushFont(headingFont);
	ImGui::Text("Projects");
	ImGui::Separator();
	ImGui::PopFont();

	for (int i = 0; i < gblProjects.size(); i++) {
		ProjectEntry project = gblProjects[i];
		projectNode(project);
	}
	ImGui::NewLine();

	if (ImGui::Button("New Project"))
		ImGui::OpenPopup("New Project");

	ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiCond_FirstUseEver);
	if (ImGui::BeginPopupModal("New Project", nullptr, ImGuiWindowFlags_NoTitleBar)) {
		ImGui::PushFont(headingFont);
		ImGui::Text("New Project");
		ImGui::Separator();
		ImGui::PopFont();

		ImGui::InputText("Project Name", &projectName, ImGuiInputTextFlags_CallbackEdit,
			projectNameEditCallback);
		ImGui::InputText("Package ID", &packageID);
		// ImGui::InputText("Project Location", &projectLocation);
		ImGui::Text("(Will create new directory %s)",
			(Shadow::EngineConfiguration::getConfigDir() + "/Projects/" + projectName).c_str());

#if __cplusplus > BX_LANGUAGE_CPP17
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::Text("%s", issue.c_str());
		ImGui::PopStyleColor();
#else
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 153, 43, 255));
		ImGui::Text("* Project name checking isn't supported in builds of Shadow using a standard "
					"lower than C++20");
		ImGui::PopStyleColor();
#endif

		if (ImGui::Button("Cancel"))
			ImGui::CloseCurrentPopup();
		ImGui::SameLine();
		ImGui::BeginDisabled(!issue.empty());
		if (ImGui::Button("Create"))
			createProject();
		ImGui::EndDisabled();
		ImGui::EndPopup();
	}

	if (ImGui::Button("Force open editor")) {
		ProjectEntry proj;
		openEditorNow(proj);
	}

	if (ImGui::Button("Force open legacy editor")) {
		openLegacyEditorAfterDeath = true;
		refWindow->close();
	}

	ImGui::End();
}

namespace Shadow {

int Editor::startProjectBrowser() {
	InitBXFilesystem();
	IMGUI_CHECKVERSION();

	int width = 1280, height = 720;

	ShadowWindow projectEditorWindow(width, height, "Project Browser");
	refWindow = &projectEditorWindow;

	bgfx::Init init;
	init.type = bgfx::RendererType::Vulkan;

	init.platformData.ndt = projectEditorWindow.getNativeDisplayHandle();
	init.platformData.nwh = projectEditorWindow.getNativeWindowHandle();

	auto bounds = projectEditorWindow.getExtent();
	width = bounds.width;
	height = bounds.height;
	init.resolution.width = (uint32_t)width;
	init.resolution.height = (uint32_t)height;
	init.resolution.reset = vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;
	if (!bgfx::init(init))
		return 1;

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x222222FF, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigDockingTransparentPayload = true;

	// TODO: in the future make this one file load into two fonts
	mainFont = io.Fonts->AddFontFromFileTTF("./Resources/caskaydia-cove-nerd-font-mono.ttf", 16.0f);
	headingFont
		= io.Fonts->AddFontFromFileTTF("./Resources/caskaydia-cove-nerd-font-mono.ttf", 40.0f);
	io.Fonts->AddFontDefault();
	io.FontGlobalScale = 1.3f;
	io.IniFilename = "./Resources/projectBrowser.ini";

	ImGui::SetupTheme();

	ImGui_Implbgfx_Init(255);
	ImGui_ImplGlfw_InitForVulkan(projectEditorWindow.window, true);

	const bgfx::Caps* rendererCapabilities = bgfx::getCaps();

	bgfx::UniformHandle u_time = bgfx::createUniform("u_time", bgfx::UniformType::Vec4);
	float speed = 0.37f;
	float time = 0.0f;

	gblProjects = getProjects();

	while (!projectEditorWindow.shouldClose()) {
		glfwPollEvents();

		if (projectEditorWindow.wasWindowResized()) {
			auto bounds = projectEditorWindow.getExtent();
			width = bounds.width;
			height = bounds.height;
			bgfx::reset(
				(uint32_t)width, (uint32_t)height, vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
			bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);
			projectEditorWindow.resetWindowResizedFlag();
		}

		ImGui_Implbgfx_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		ImGui::ShowDemoWindow();

		drawProjectBrowser();

		ImGui::Render();
		ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

		bgfx::touch(0);

		int64_t now = bx::getHPCounter();
		static int64_t last = now;
		const int64_t frameTime = now - last;
		last = now;
		const double freq = double(bx::getHPFrequency());
		const float deltaTime = float(frameTime / freq);

		time += (float)(frameTime + speed / freq);

		bgfx::setUniform(u_time, &time);

		{
			// Blank background matrix
			const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
			const bx::Vec3 eye = { 0.0f, 0.0f, 0.0f };
			float bgViewMatrix[16];
			bx::mtxLookAt(bgViewMatrix, eye, at);
			float bgProjectionMatrix[16];
			bx::mtxProj(bgProjectionMatrix, 60.0f, float(width) / float(height), 0.1f, 100.0f,
				bgfx::getCaps()->homogeneousDepth);
			bgfx::setViewTransform(0, bgViewMatrix, bgProjectionMatrix);

			bgfx::touch(0);
		}

		bgfx::frame();
	}

	bgfx::destroy(u_time);

	for (int i = 0; i < gblProjects.size(); i++) {
		bgfx::destroy(gblProjects[i].icon);
	}

	ImGui_ImplGlfw_Shutdown();
	ImGui_Implbgfx_Shutdown();

	ShutdownBXFilesytem();

	ImGui::DestroyContext();
	bgfx::shutdown();
	projectEditorWindow.shutdown();

	if (openEditorAfterDeath) {
		Shadow::startEditor(editorNowProject);
	}

	if (openLegacyEditorAfterDeath) {
		Shadow::StartRuntime();
	}

	return 0;
}

}