#include "CommandCenterBackend.hpp"
#include "Core.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "Editor/ContentBrowser.hpp"
#include "Editor/Editor.hpp"
#include "Editor/EditorParts/EditorParts.hpp"
#include "Editor/Project.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Scene/FlyCamera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneSerializer.hpp"
#include "ShadowWindow.hpp"
#include "Util.hpp"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "bx/math.h"
#include "bx/timer.h"
#include "imgui.h"
#include "imgui/theme.hpp"
#include "imgui/imgui_impl_bgfx.h"
#include "imgui_impl_glfw.h"
#include <csignal>
#include <cstdint>
#include "../../lib/bgfx/3rdparty/dear-imgui/widgets/gizmo.h"
#include "../AXE/IconsCodicons.h"

#define EDITOR_UI_VIEW_ID 2
#define EDITOR_VIEWPORT_VIEW_ID 10

static bool vsync = false;

static Shadow::ShadowWindow* editorWindowReference;
static Shadow::Reference<Shadow::Scene> editorScene;

static float viewportViewMatrix[16];
static float viewportProjectionMatrix[16];

static void sigintHandler(int signal) {
	BX_UNUSED(signal);
	editorWindowReference->close();
}

static void resetViews() {
	// bgfx::setViewClear(EDITOR_VIEWPORT_VIEW_ID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x111111FF, 1.0f, 0);
	bgfx::setViewClear(EDITOR_UI_VIEW_ID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x111111FF, 1.0f, 0);
	// bgfx::setViewRect(EDITOR_VIEWPORT_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);
	bgfx::setViewRect(EDITOR_UI_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);
}

namespace Shadow {

void loadScene(const std::string &sceneFilePath) {
	SceneSerializer ss(editorScene);
	ss.deserialize(sceneFilePath);
}

Reference<Scene> getCurrentScene() {
	return editorScene;
}

int startEditor(Shadow::Editor::ProjectEntry project) {
	Editor::setCurrentProjectName(project.name);
	Editor::setCurrentProjectPath(project.path);

	EC_NEWCAT("Editor");

	ShadowWindow editorWindow(1800, 900, "Shadow Editor");
	editorWindowReference = &editorWindow;
	std::signal(SIGINT, sigintHandler);

	CMD_CTR_NEWCMD("Text Editor: Open", "Bruh", []() {
		WARN("TEXT EDITOR OPEN");
		
	});

	CMD_CTR_NEWCMD("Scene: Open Scene in Text Editor", "desc", []() {
		WARN("OPENED SCENE IN TEXT EDITOR");
	});
	
	bgfx::Init init;
	init.type = bgfx::RendererType::Vulkan;
	init.platformData.ndt = editorWindow.getNativeDisplayHandle();
	init.platformData.nwh = editorWindow.getNativeWindowHandle();
	init.resolution.width = (uint32_t)editorWindow.getExtent().width;
	init.resolution.height = (uint32_t)editorWindow.getExtent().height;
	init.resolution.reset = vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;
	bgfx::init(init);

	resetViews();
	bgfx::setViewClear(EDITOR_VIEWPORT_VIEW_ID, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x111111FF, 1.0f, 0);
	bgfx::setViewRect(EDITOR_VIEWPORT_VIEW_ID, 0, 0, bgfx::BackbufferRatio::Equal);

	Mouse mouse(&editorWindow);
	Keyboard keyboard(&editorWindow);

	// * time uniform
	bgfx::UniformHandle u_time = bgfx::createUniform("u_time", bgfx::UniformType::Vec4);

	// * picking uniforms
	bgfx::UniformHandle u_tint = bgfx::createUniform("u_tint", bgfx::UniformType::Vec4);
	bgfx::UniformHandle u_id = bgfx::createUniform("u_id", bgfx::UniformType::Vec4);

	// * viewport texture and buffer
	bgfx::TextureHandle viewportTexture = bgfx::createTexture2D(
		(uint16_t)editorWindow.getExtent().width,
		(uint16_t)editorWindow.getExtent().height,
		false, 1, bgfx::TextureFormat::BGRA8,
		BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT
	);
	bgfx::FrameBufferHandle viewportFrameBuffer = bgfx::createFrameBuffer(1, &viewportTexture, true);

	editorScene = Shadow::CreateReference<Shadow::Scene>();

	{ // * ImGui Init
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |=	ImGuiConfigFlags_DockingEnable
					|	ImGuiConfigFlags_NavEnableKeyboard
					|	ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigDockingTransparentPayload = true;

		// Font loading and scaling
		{
			// IMGUI_ENABLE_FREETYPE in imconfig to use Freetype for higher quality font rendering
			float sf = editorWindow.getContentScale();

			float fontSize = 16.0f * sf;
			float iconFontSize = (fontSize * 2.0f / 3.0f) * sf;

			ImVector<ImWchar> ranges;
			ImFontGlyphRangesBuilder builder;
			builder.AddText((const char*)u8"♭");

			ImFontConfig fontCfg;
			fontCfg.OversampleH = 4;
			fontCfg.OversampleV = 4;
			fontCfg.PixelSnapH = false;

			ImFont* primaryFont = io.Fonts->AddFontFromFileTTF("./Resources/caskaydia-cove-nerd-font-mono.ttf", fontSize, &fontCfg, ranges.Data);
			// ImFont* primaryFont = io.Fonts->AddFontFromFileTTF("./Resources/arial.ttf", fontSize, &fontCfg, ranges.Data);

			static const ImWchar iconRanges[] = { ICON_MIN_CI, ICON_MAX_CI, 0 };

			ImFontConfig iconFontCfg;
			iconFontCfg.GlyphMinAdvanceX = iconFontSize;
			iconFontCfg.MergeMode = true;
			iconFontCfg.PixelSnapH = true;
			iconFontCfg.OversampleH = 2;
			iconFontCfg.OversampleV = 2;
			iconFontCfg.GlyphOffset.y = 6;
			iconFontCfg.DstFont = primaryFont;

			io.Fonts->AddFontFromFileTTF("./Resources/codicon.ttf", 20.0f * sf, &iconFontCfg, iconRanges);

			ImGui::GetStyle().ScaleAllSizes(sf);
		}

		ImGui::SetupTheme();

		ImGui_Implbgfx_Init(EDITOR_UI_VIEW_ID);
		ImGui_ImplGlfw_InitForVulkan(editorWindow.window, true);
		
		Editor::EditorParts::init({
			&editorWindow,
			&keyboard,
			&viewportTexture,
			editorScene,
			viewportViewMatrix,
			viewportProjectionMatrix
		});
	}

	ContentBrowser contentBrowser;

	// bgfx::ProgramHandle program = loadProgram("test/vs_test.sc.spv", "test/fs_test.sc.spv");
	// bgfx::ProgramHandle program = loadProgram("mesh/vs_mesh.sc.spv", "mesh/fs_mesh.sc.spv");

	// * camera init
	SceneNamespace::FlyCamera flyCamera = SceneNamespace::FlyCamera(&mouse, &keyboard);
	flyCamera.setPosition({0.0f, 0.0f, -15.0f });
	flyCamera.setVerticalAngle(0.0f);

	while (!editorWindow.shouldClose()) {
		editorWindow.pollEvents();

		if (editorWindow.wasWindowResized()) {
			bgfx::reset(
				(uint32_t)editorWindow.getExtent().width,
				(uint32_t)editorWindow.getExtent().height,
				vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
			
			resetViews();

			editorWindow.resetWindowResizedFlag();
		}

		mouse.onUpdate();

		{
			ImGui_Implbgfx_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGuizmo::BeginFrame();
		}

		Editor::EditorParts::onUpdate();
		EditorConsole::Frontend::onUpdate();

		contentBrowser.onUpdate();

		{
			ImGui::Render();
			ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
		}
		
		{ // * Time calculations and u_time updating
			float speed = 0.37f, time = 0.0f;
			int64_t now = bx::getHPCounter();
			static int64_t last = now;
			const int64_t frameTime = now - last;
			last = now;
			const double freq = double(bx::getHPFrequency());
			const float deltaTime = float(frameTime / freq);
			time += (float)(frameTime + speed / freq);
			bgfx::setUniform(u_time, &time);

			// * FlyCamera update
			flyCamera.update(deltaTime, !Editor::EditorParts::isMouseOverViewport());
			// flyCamera.update(deltaTime, false);
		}


		{ // * Viewport matrix updating
			flyCamera.getViewMtx(viewportViewMatrix);

			bx::mtxProj(viewportProjectionMatrix, 60.0f,
				Editor::EditorParts::getViewportWidth() / Editor::EditorParts::getViewportHeight(),
				0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

			bgfx::setViewName(EDITOR_VIEWPORT_VIEW_ID, "Editor Viewport");
			bgfx::setViewTransform(EDITOR_VIEWPORT_VIEW_ID, viewportViewMatrix, viewportProjectionMatrix);
			bgfx::setViewFrameBuffer(EDITOR_VIEWPORT_VIEW_ID, viewportFrameBuffer);

			bgfx::touch(EDITOR_VIEWPORT_VIEW_ID);
		}

		editorScene->onUpdate(EDITOR_VIEWPORT_VIEW_ID);

		bgfx::frame();
	}

	Editor::EditorParts::destroy();

	contentBrowser.unload();

	bgfx::destroy(u_time);
	bgfx::destroy(u_tint);
	bgfx::destroy(u_id);
	bgfx::destroy(viewportFrameBuffer);
	// bgfx::destroy(program);


	editorScene->unload();

	ImGui_ImplGlfw_Shutdown();
	ImGui_Implbgfx_Shutdown();
	ImGui::DestroyContext();

	bgfx::shutdown();
	editorWindow.shutdown();

	return 0;
}

}
