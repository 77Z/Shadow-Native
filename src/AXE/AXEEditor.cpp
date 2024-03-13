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
#include <cstdint>
#include <string>
#include "Debug/EditorConsole.hpp"

#include "ImZoomSlider.h"

namespace Shadow::AXE {

static float uMin = 0.4f, uMax = 0.6f;
static float staticDuration = 10.0f;
static bool autoDuration = true;

class Track {
public:
	Track() {}
	~Track() {}

	void drawTrack(uint32_t visualIndex) {
		// Visual index is only used so the track knows where it is on the screen

		ImGui::PushID(visualIndex);

		const ImVec2 sliderSize(18.0f, 120.0f);

		ImGui::SliderFloat("VOLUME", &volume, 0, 100);

		const char* volAdjustPopupName = std::string("volumeadjust").append(std::to_string(visualIndex)).c_str();

		if (ImGui::BeginPopup(volAdjustPopupName)) {
			// ImGui::PushID(-visualIndex);
			ImGui::InputFloat("Volume", &volume);
			// ImGui::PopID();
			ImGui::EndPopup();
		}

		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));
		ImGui::VSliderFloat("##Volume", sliderSize, &volume, 0.0f, 100.0f, "\n\nV\nO\nL");
		ImGui::PopStyleColor();
		if (ImGui::IsItemActive() || ImGui::IsItemHovered()) ImGui::SetTooltip("Volume: %.2f%%\n(CTRL Click to set)", volume);
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::GetIO().KeyCtrl) ImGui::OpenPopup(volAdjustPopupName);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));
		ImGui::VSliderFloat("##Balence", sliderSize, &balence, -1.0f, 1.0f, "R\n\nB\nA\nL\n\nL");
		ImGui::PopStyleColor();
		if (ImGui::IsItemActive() || ImGui::IsItemHovered()) ImGui::SetTooltip("Balence %.2f\n(CTRL Click to set)", balence);

		ImGui::PopID();
	}

	float balence = 0;
	float volume = 100;
};

int startAXEEditor(AXEProjectEntry project) {
	ShadowWindow axeEditorWindow(1820, 1280, "AXE Editor");
	RenderBootstrapper rb(&axeEditorWindow, bgfx::RendererType::Vulkan, false);

	uint32_t frame = 0;

	Track t1;
	Track t2;

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

		const float reservedSpace = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		if (ImGui::BeginChild("TimelineRegion", ImVec2(0, -reservedSpace), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
			
			t1.drawTrack(0);
			t2.drawTrack(2);

			ImGui::EndChild();
		}

		ImZoomSlider::ImZoomSlider(0.f, staticDuration, uMin, uMax);


		ImGui::End();

		EditorConsole::Frontend::onUpdate();

		rb.endFrame();

		if (frame == 300) {
			axeEditorWindow.postEmptyEvent();
		}

		frame++;
	}

	rb.shutdown();

	return 0;
}

}
