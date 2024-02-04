#include "Editor/EditorParts/EditorParts.hpp"
#include "Editor/Project.hpp"
#include "Util.hpp"
#include "bgfx/bgfx.h"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "imgui_internal.h"

namespace Shadow::Editor::EditorParts {

static bgfx::TextureHandle shadowLogo;

void rootWindowInit() {
	shadowLogo = loadTexture("./Resources/logo.png");
}

void rootWindowDestroy() {
	bgfx::destroy(shadowLogo);
}

static void drawMainMenuBar() {
	if (ImGui::myBeginMenuBar()) {
		ImGui::SetCursorPosX(100);

		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Asset", "CTRL + N")) { }
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "CTRL + SHIFT + Q")) {
				// refWindow->close();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "CTRL + Z")) { }
			if (ImGui::MenuItem("Redo", "CTRL + Y", false, false)) { }
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL + X")) { }
			if (ImGui::MenuItem("Copy", "CTRL + C")) { }
			if (ImGui::MenuItem("Paste", "CTRL + V")) { }
			ImGui::Separator();
			if (ImGui::MenuItem("Project Preferences", "CTRL + SHIFT + ,")) {
				Shadow::Editor::EditorParts::showProjectPreferences();
			}
			if (ImGui::MenuItem("Engine Preferences", "CTRL + ,")) {
				Shadow::Editor::EditorParts::showEnginePreferences();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Build")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Run")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {
			ImGui::Text("If you need help, just text me");

			ImGui::Separator();
			
			if (ImGui::MenuItem("Shadow Engine Website")) {
				Shadow::Util::openURL("https://shadow.77z.dev");
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

static float drawTitlebar() {
	const float titlebarHeight = 58.0f;
	const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;

	ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y));

	const ImVec2 titlebarMin = ImGui::GetCursorScreenPos();
	const ImVec2 titlebarMax
		= { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - windowPadding.y * 2.0f,
			  ImGui::GetCursorScreenPos().y + titlebarHeight };
	auto* bgDrawList = ImGui::GetBackgroundDrawList();
	auto* fgDrawList = ImGui::GetForegroundDrawList();
	bgDrawList->AddRectFilled(titlebarMin, titlebarMax, IM_COL32(255, 0, 0, 255));
	// DEBUG TITLEBAR BOUNDS
	fgDrawList->AddRect(titlebarMin, titlebarMax, IM_COL32(255, 255, 0, 255));

	{ // * Logo
		const int logoWidth = 48;
		const int logoHeight = 48;
		const ImVec2 logoOffset(windowPadding.x + 16.0f, windowPadding.y + 5.0f);
		const ImVec2 logoRectStart(ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y);
		const ImVec2 logoRectMax(logoRectStart.x + logoWidth, logoRectStart.y + logoHeight);

		fgDrawList->AddImage(ImGui::toId(shadowLogo, 0, 0), logoRectStart, logoRectMax);
	}

	drawMainMenuBar();

	// * Project title
	std::string projectName = Editor::getCurrentProjectName();
	ImVec2 projectNameTextBounds = ImGui::CalcTextSize(projectName.c_str());
	ImGui::SetCursorPos(
		ImVec2(titlebarMax.x - projectNameTextBounds.x - 10.0f,
		titlebarMax.y - projectNameTextBounds.y - 10.0f
	));
	ImGui::Text("%s", projectName.c_str());

	// ? Do we even need this? The window title should already be drawn by OS decorations?
	/* { // * Window title
		const char* title = refWindow->windowTitle.c_str();
		ImVec2 currentCursorPos = ImGui::GetCursorPos();
		ImVec2 textSize = ImGui::CalcTextSize(title);
		fgDrawList->AddText(ImVec2(ImGui::GetWindowWidth() * 0.5f - textSize.x * 0.5f,
								2.0f + windowPadding.y + 6.0f),
			IM_COL32(255, 255, 255, 255), title);
		ImGui::SetCursorPos(currentCursorPos);
	} */

	return titlebarHeight;
}

void rootWindowUpdate() {
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport  = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));

	//TODO: Move these props up to initial windowFlags init?
	windowFlags |=
		  ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_MenuBar;
	
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 255));

	ImGui::Begin("Shadow Editor Root Window", nullptr, windowFlags);

	ImGui::PopStyleColor();
	ImGui::PopStyleVar(3);

	float titleBarHeight = drawTitlebar();
	ImGui::SetCursorPosY(titleBarHeight);

	ImGui::DockSpace(ImGui::GetID("Shadow Editor Dockspace"));

	ImGui::End();
}

}