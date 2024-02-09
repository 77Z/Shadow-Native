#include "AXE/AXEEditor.hpp"
#include "Debug/Logger.hpp"
#include "ImCurveEdit.h"
#include "ImSequencer.h"
#include "RenderBootstrapper.hpp"
#include "ShadowWindow.hpp"
#include "bgfx/bgfx.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <algorithm>
#include <cstring>
#include <vector>

#include "ImZoomSlider.h"

namespace Shadow::AXE {

static float uMin = 0.4f, uMax = 0.6f;
static float staticDuration = 10.0f;
static bool autoDuration = true;

int startAXEEditor(AXEProjectEntry project) {
	ShadowWindow axeEditorWindow(1820, 1280, "AXE Editor");
	RenderBootstrapper rb(&axeEditorWindow, bgfx::RendererType::Vulkan);

	while (!axeEditorWindow.shouldClose()) {
		//axeEditorWindow.pollEvents();
    axeEditorWindow.waitEvents();

		rb.startFrame();

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

		ImGui::Text("Hello");

		ImGui::SameLine();

		ImGui::BeginDisabled(autoDuration);
		ImGui::PushItemWidth(100.0f);
		ImGui::InputFloat("Duration", &staticDuration, 1.0f, 10.0f);
		ImGui::PopItemWidth();
		ImGui::EndDisabled();
		ImGui::SameLine();
		ImGui::Checkbox("Auto Duration", &autoDuration);

		ImGui::SetCursorPosY(70.0f);

		ImGui::DockSpace(ImGui::GetID("MyDockspace"));

		ImGui::End();

		ImGui::ShowMetricsWindow();
		ImGui::ShowDemoWindow();

		ImGui::Begin("Timeline");
		ImGui::Text("%s", project.name.c_str());
		ImGui::Text("%s", project.path.c_str());
		ImGui::Text("%.3f", uMin);
		ImGui::Text("%.3f", uMax);
		ImZoomSlider::ImZoomSlider(0.f, staticDuration, uMin, uMax);

		ImGui::End();

		rb.endFrame();
	}

	rb.shutdown();

	return 0;
}

}
