#include "AXE/AXEEditor.hpp"
#include "Debug/Logger.hpp"
#include "ImCurveEdit.h"
#include "ImSequencer.h"
#include "RenderBootstrapper.hpp"
#include "ShadowWindow.hpp"
#include "Util.hpp"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bimg/bimg.h"
#include "bx/error.h"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "imgui_internal.h"
#include <algorithm>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "Debug/EditorConsole.hpp"
#include "bimg/decode.h"

// #include "ImZoomSlider.h"

namespace Shadow::AXE {

static float uMin = 0.4f, uMax = 0.6f;
static float staticDuration = 10.0f;
static bool autoDuration = true;

class Track {
public:
	Track() {}
	~Track() {}

	void drawTrack(size_t visualIndex, std::vector<Track>* tracks) {
		// Visual index is only used so the track knows where it is on the screen

		ImGui::PushID(visualIndex);

		const ImVec2 sliderSize(18.0f, 120.0f);

		// ImGui::SliderFloat("VOLUME", &volume, 0, 100);

		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
		ImGui::PushItemWidth(100.0f);
		ImGui::InputText("##trackname", &trackName);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::SmallButton("X")) {
			tracks->erase(tracks->begin() + visualIndex);
		}
		ImGui::PopStyleColor(2);

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

	std::string trackName = "Untitled Track";

	float balence = 0;
	float volume = 100;
};

static std::vector<Track> tracks;

void updateTexture(bgfx::TextureHandle& handle, bimg::ImageContainer* imgContainer) {
	const bgfx::Memory* imgMem = bgfx::makeRef(imgContainer->m_data, imgContainer->m_size, imageReleaseCb, imgContainer);
	bgfx::updateTexture2D(handle, 0, 0, 0, 0, uint16_t(800), uint16_t(600), imgMem);
}
/* 
int startAXEEditor(AXEProjectEntry project) {
	ShadowWindow axeEditorWindow(1500, 1000, "AXE Editor");
	RenderBootstrapper rb(&axeEditorWindow, bgfx::RendererType::Vulkan, false);

	uint32_t frame = 0;

	Track t1;
	Track t2;
	tracks.push_back(t1);
	tracks.push_back(t2);

	using namespace boost::interprocess;
	shared_memory_object shm(open_or_create, "axe_link_shm", read_only);
	mapped_region region(shm, read_only);
	bimg::ImageContainer* imgContainer = bimg::imageParse(getAllocator(), region.get_address(), region.get_size());
	const bgfx::Memory* imgMem = bgfx::makeRef(imgContainer->m_data, imgContainer->m_size, imageReleaseCb, imgContainer);
	bgfx::TextureHandle handle = bgfx::createTexture2D(
		uint16_t(800),
		uint16_t(600),
		false,
		1,
		bgfx::TextureFormat::Enum(imgContainer->m_format),
		BGFX_TEXTURE_RT,
		imgMem);

	while (!axeEditorWindow.shouldClose()) {
		axeEditorWindow.pollEvents();
		// axeEditorWindow.waitEvents();

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
			
			for (size_t i = 0; i < tracks.size(); i++) {
				tracks[i].drawTrack(i, &tracks);
			}

			if (ImGui::Button("+ Add Track")) {
				Track temp;
				tracks.push_back(temp);
			}

		}
			ImGui::EndChild();

		// ImZoomSlider::ImZoomSlider(0.f, staticDuration, uMin, uMax);


		ImGui::End();

		ImGui::Begin("AXE UI WINDOW");

		ImGui::Spinner("##spinner", 25, 10, IM_COL32(255, 0, 0, 255));
		ImGui::SameLine();
		ImGui::Text("    Connecting to AXE");

		ImGui::Image(
			handle,
			ImVec2(800, 600)
		);

		updateTexture(handle, imgContainer);

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
} */

}
