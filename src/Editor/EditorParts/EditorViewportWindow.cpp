#include "Editor/Editor.hpp"
#include "Editor/EditorParts/EditorParts.hpp"
#include "ShadowWindow.hpp"
#include "bgfx/bgfx.h"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "ImGuizmo.h"

namespace Shadow::Editor::EditorParts {

static bool mouseOverViewport = false;

static const float* viewportViewMatrix;
static const float* viewportProjectionMatrix;

// Dummy values
static ImVec2 vportMin(0, 0);
static ImVec2 vportMax(800, 450);

static float vportWidth = vportMax.x - vportMin.x;
static float vportHeight = vportMax.y - vportMin.y;

float getViewportWidth() { return vportWidth; }
float getViewportHeight() { return vportHeight; }
bool isMouseOverViewport() { return mouseOverViewport; }

static bgfx::TextureHandle* viewportTextureRef;
static ShadowWindow* editorWindowRef;
static Reference<Shadow::Scene> editorSceneRef;

void viewportWindowInit(EditorPartsCarePackage carePackage) {
	viewportTextureRef = carePackage.viewportTextureRef;
	editorWindowRef = carePackage.editorWindowRef;
	editorSceneRef = carePackage.editorSceneRef;
	viewportViewMatrix = carePackage.viewportViewMatrix;
	viewportProjectionMatrix = carePackage.viewportProjectionMatrix;
}

void viewportWindowUpdate() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiCond_FirstUseEver);
	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse);

	mouseOverViewport = ImGui::IsWindowHovered();

	ImGuiIO& io = ImGui::GetIO();

	if (ImGui::IsWindowHovered()) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			editorWindowRef->lockCursor();
			io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}
	}

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		editorWindowRef->unlockCursor();
		io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
	}

	vportMin = ImGui::GetWindowContentRegionMin();
	vportMax = ImGui::GetWindowContentRegionMax();

	vportMin.x += ImGui::GetWindowPos().x;
	vportMin.y += ImGui::GetWindowPos().y;
	vportMax.x += ImGui::GetWindowPos().x;
	vportMax.y += ImGui::GetWindowPos().y;

	vportWidth = vportMax.x - vportMin.x;
	vportHeight = vportMax.y - vportMin.y;

	// ImGui::GetForegroundDrawList()->AddRect(vportMin, vportMax, IM_COL32(255, 255, 0, 255));

	ImGui::Image(*viewportTextureRef, ImVec2(vportWidth, vportHeight));

	float translation[3] = { 0.0f, 0.0f, 0.0f };
	float rotation[3] = { 0.0f, 0.0f, 0.0f };
	float scale[3] = { 1.0f, 1.0f, 1.0f };
	float mtx[16];
	ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, mtx);
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(viewportViewMatrix, viewportProjectionMatrix, ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, mtx);

	ImGui::SetCursorPos(ImVec2(10, 30));
	ImGui::Text("Scene: %s", editorSceneRef.get()->sceneName.c_str());
	ImGui::SetCursorPosX(10);
	ImGui::Text("View Matrix [0]: %.3f", viewportViewMatrix[0]);

	ImGui::End();
	ImGui::PopStyleVar(2);
}

}