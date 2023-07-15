#include "Editor/ContentBrowser.hpp"
#include "Util.h"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "imgui.h"
#include "imgui/imgui_utils.h"
#include "imgui_internal.h"

namespace ImGui {
namespace ContentBrowser {

	bool fileNode(const char* label, ImTextureID icon) {

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size(100, 130);
		ImRect boundingBox(pos, ImVec2(pos.x + size.x, pos.y + size.y));

		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		ItemSize(size);
		if (!ItemAdd(boundingBox, id))
			return false;

		// Render
		bool hovered, held;
		bool pressed = ButtonBehavior(boundingBox, id, &hovered, &held);

		if (hovered && ImGui::BeginTooltip()) {
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(56, 76, 255, 255));
			ImGui::TextUnformatted("C++ Source File");
			ImGui::PopStyleColor();
			ImGui::Separator();
			ImGui::Text("%s", label);
			ImGui::EndTooltip();
		}

		const ImU32 color
			= GetColorU32(hovered ? IM_COL32(34, 34, 34, 255) : IM_COL32(20, 20, 20, 255));
		RenderNavHighlight(boundingBox, id);
		RenderFrame(boundingBox.Min, boundingBox.Max, color, true, 3.0f);

		RenderTextClipped(ImVec2(boundingBox.Min.x, boundingBox.Min.y + 100),
			ImVec2(boundingBox.Max.x, boundingBox.Max.y + 100), label, NULL, &label_size,
			ImVec2(0, 0), &boundingBox);

		window->DrawList->AddImage(
			icon, boundingBox.Min, ImVec2(boundingBox.Max.x, boundingBox.Max.y - 30));

		// GetForegroundDrawList()->AddRect(
		// 	boundingBox.Min, boundingBox.Max, IM_COL32(255, 0, 0, 255));

		ImGui::SameLine();
		return pressed;
	}

}
}

namespace Shadow {

ContentBrowser::ContentBrowser() {
	// const bgfx::Memory* mem = bgfx::alloc(270000);
	// unsigned char* texbuf = (unsigned char*)mem->data;

	// bx::memSet(texbuf, 0xff, mem->size);

	// fileIcon = bgfx::createTexture2D(
	// 	300, 300, false, 1, bgfx::TextureFormat::RGB8, BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, mem);

	fileIcon = loadTexture("./fileIcon.png");
}

ContentBrowser::~ContentBrowser() { }

void ContentBrowser::unload() { bgfx::destroy(fileIcon); }

void ContentBrowser::onUpdate() {
	ImGui::SetNextWindowSize(ImVec2(1600, 200), ImGuiCond_FirstUseEver);
	ImGui::Begin("Content Browser");

	// ImGui::Image(fileIcon, ImVec2(70, 70));

	// ImGui::Button("Rawww");
	ImGui::ContentBrowser::fileNode("File1.cppppppppp", ImGui::toId(fileIcon, 0, 0));
	ImGui::ContentBrowser::fileNode("File2.cpp", ImGui::toId(fileIcon, 0, 0));
	// ImGui::ContentBrowser::fileNode("File2.cpp");
	// ImGui::Button("Bruh");

	ImGui::End();
}

}