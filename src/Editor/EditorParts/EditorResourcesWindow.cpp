#include "Editor/EditorParts/EditorParts.hpp"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "dummy.hpp"
#include "imgui.h"
#include <cstdint>

namespace Shadow::Editor::EditorParts {

static const bgfx::Caps* caps;
static const bgfx::Stats* stats;

void resourcesWindowInit() {
	caps = bgfx::getCaps();
	stats = bgfx::getStats();
}

static bool bar(float width, float maxWidth, float height, const ImVec4& color) {
	const ImGuiStyle& style = ImGui::GetStyle();

	ImVec4 hoveredColor(
		  color.x + color.x * 0.1f
		, color.y + color.y * 0.1f
		, color.z + color.z * 0.1f
		, color.w + color.w * 0.1f
		);

	ImGui::PushStyleColor(ImGuiCol_Button,        color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,  color);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, style.ItemSpacing.y) );

	bool hovered = false;

	ImGui::Button("##", ImVec2(width, height) );
	hovered |= ImGui::IsItemHovered();

	ImGui::SameLine();
	ImGui::InvisibleButton("##", ImVec2(bx::max(1.0f, maxWidth - width), height) );
	hovered |= ImGui::IsItemHovered();

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(3);

	return hovered;
}

static const ImVec4 resourceColor(0.5f, 0.5f, 0.5f, 1.0f);

static void resourceBar(const char* name, const char* tooltip, uint32_t num, uint32_t max, float maxWidth, float height) {
	bool hovered = false;

	ImGui::Text("%s: %4d / %4d", name, num, max);
	hovered |= ImGui::IsItemHovered();
	ImGui::SameLine();

	const float percentage = float(num) / float(max);

	hovered |= bar(bx::max(1.0f, percentage * maxWidth), maxWidth, height, resourceColor);
	ImGui::SameLine();

	ImGui::Text("%5.2f%%", percentage * 100.0f);

	if (hovered)
		ImGui::SetTooltip("%s %5.2f%%", tooltip, percentage * 100.0f);
}

void resourcesWindowUpdate() {

	ImGui::Begin("Allocated Resources");

	const float itemHeight = ImGui::GetTextLineHeightWithSpacing();
	const float maxWidth = 90.0f;

	ImGui::Text("Res: Num  / Max");
	resourceBar("DIB", "Dynamic Index Buffers",	stats->numDynamicIndexBuffers,	caps->limits.maxDynamicIndexBuffers,	maxWidth, itemHeight);
	resourceBar("DVB", "Dynamic vertex buffers",	stats->numDynamicVertexBuffers,	caps->limits.maxDynamicVertexBuffers,	maxWidth, itemHeight);
	resourceBar(" FB", "Frame buffers",			stats->numFrameBuffers,			caps->limits.maxFrameBuffers,			maxWidth, itemHeight);
	resourceBar(" IB", "Index buffers",			stats->numIndexBuffers,			caps->limits.maxIndexBuffers,			maxWidth, itemHeight);
	resourceBar(" OQ", "Occlusion queries",		stats->numOcclusionQueries,		caps->limits.maxOcclusionQueries,		maxWidth, itemHeight);
	resourceBar("  P", "Programs",				stats->numPrograms,				caps->limits.maxPrograms,				maxWidth, itemHeight);
	resourceBar("  S", "Shaders",				stats->numShaders,				caps->limits.maxShaders,				maxWidth, itemHeight);
	resourceBar("  T", "Textures",				stats->numTextures,				caps->limits.maxTextures,				maxWidth, itemHeight);
	resourceBar("  U", "Uniforms",				stats->numUniforms,				caps->limits.maxUniforms,				maxWidth, itemHeight);
	resourceBar(" VB", "Vertex buffers",			stats->numVertexBuffers,		caps->limits.maxVertexBuffers,			maxWidth, itemHeight);
	resourceBar(" VL", "Vertex layouts",			stats->numVertexLayouts,		caps->limits.maxVertexLayouts,			maxWidth, itemHeight);

	if (ImGui::Button("Dummy func")) {
		Shadow::dummy();
	}

	ImGui::End();
}

}