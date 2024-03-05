#include "ModelViewer.hpp"
#include "RenderBootstrapper.hpp"
#include "ShadowWindow.hpp"
#include "bgfx/bgfx.h"
#include "imgui.h"

namespace Shadow {
	// ! This being a class implies that you can summon multiple of these. you
	// ! can. However the constructor is blocking so the user of said class
	// ! would have to be responsible for spinning a new thread for this class
	// ! to operate on. Not really practical.
	ModelViewer::ModelViewer(const std::string& filePath) {
		ShadowWindow modelViewerWindow(800, 1200, "Shadow Engine Model Viewer");
		RenderBootstrapper rb(&modelViewerWindow, bgfx::RendererType::Vulkan);

		while (!modelViewerWindow.shouldClose()) {
			modelViewerWindow.waitEvents();
			rb.startFrame();

			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

			ImGui::Begin("Model Viewport");
			ImGui::Text("%s", filePath.c_str());
			ImGui::End();

			ImGui::Begin("Model Settings");
			if (ImGui::BeginTabBar("mdl-settings-tabbar")) {

				if (ImGui::BeginTabItem("Render")) {
					ImGui::Checkbox("Wireframe", &wireframe);
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Mesh Properties")) {
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
			ImGui::End();

			rb.endFrame();
		}

		rb.shutdown();
	}

	ModelViewer::~ModelViewer() {}

}