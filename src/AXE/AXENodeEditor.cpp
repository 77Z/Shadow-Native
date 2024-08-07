#include "AXENodeEditor.hpp"
#include "AXETypes.hpp"
#include "imgui.h"
#include "IconsCodicons.h"
#include "imgui/imgui_utils.hpp"
#include <string>
#include "ppk_assert_impl.hpp"

namespace Shadow::AXE {

AXENodeEditor::AXENodeEditor(Song* song): song(song) {
	// Save settings inside axe file in future
	ed::Config config;
	config.SettingsFile = nullptr;
	config.NavigateButtonIndex = 2;
	editorCtx = ed::CreateEditor(&config);

	ed::SetCurrentEditor(editorCtx);

	auto& colors = ed::GetStyle().Colors;
	colors[ed::StyleColor_Bg] = ImColor(0, 0, 0, 255);
	colors[ed::StyleColor_HovNodeBorder] = ImColor(255, 69, 0, 255);
	colors[ed::StyleColor_SelNodeBorder] = ImColor(255, 0, 0, 255);
	colors[ed::StyleColor_PinRect] = ImColor(255, 0, 0, 255);

	ed::SetCurrentEditor(nullptr);
}

AXENodeEditor::~AXENodeEditor() { }

void AXENodeEditor::updateDebugMenu(bool& p_open) {
	using namespace ImGui;

	if (!p_open) return;

	Begin("Node Editor Diagnostics", &p_open);

	if (!openedNodeGraph) {
		TextUnformatted("No Node Graph opened yet!");
		End();
		return;
	}

	TextUnformatted("Currently opened node graph: ");
	Text("%s", openedNodeGraph->name.c_str());

	TextUnformatted("Links:");
	for (auto& link : openedNodeGraph->links) {
		Bullet();
		Text("Link #%lu - Connects %lu to %lu", link.id.Get(), link.inputId.Get(), link.outputId.Get());
	}

	TextUnformatted("Nodes:");
	for (auto& node : openedNodeGraph->nodes) {
		Bullet();
		Text("Node #%lu, %s", node.id.Get(), node.name.c_str());
		for (auto& pin : node.inputs) {
			Indent();
			Text("Input pin #%lu", pin.id.Get());
			Unindent();
		}
		for (auto& pin : node.outputs) {
			Indent();
			Text("Output pin #%lu", pin.id.Get());
			Unindent();
		}
	}

	End();
}

void AXENodeEditor::onUpdate(bool& p_open) {
	using namespace ImGui;

	if (!p_open) return;

	Begin("Node Editor", &p_open, ImGuiWindowFlags_MenuBar);
	ed::SetCurrentEditor(editorCtx);

	if (BeginMenuBar()) {
		if (MenuItem(ICON_CI_MENU " Menu")) mainMenuOpen = !mainMenuOpen;
		if (BeginMenu("+ Add")) {
			SeparatorText("Essential");
			if (MenuItem("Input")) SpawnInputNode();
			if (MenuItem("Output")) SpawnOutputNode();
			SeparatorText("Effects");
			if (MenuItem("Low Pass Filter")) SpawnLowPassFilterNode();
			if (MenuItem("Delay / Echo")) SpawnDelayNode();
			EndMenu();
		}
		if (BeginMenu("Position")) {
			if (MenuItem("Recenter", "F")) ed::NavigateToContent();
			EndMenu();
		}
		if (!openedNodeGraph) {
			TextUnformatted(" | NO NODE GRAPH OPENED ");
		} else {
			TextUnformatted(" |");
			PushItemWidth(300.0f);
			PushStyleColor(ImGuiCol_FrameBg, GetStyle().Colors[ImGuiCol_MenuBarBg]);
			InputText("##nodegraphname", &openedNodeGraph->name);
			PopStyleColor();
		}
		Text("| %.0f%%", ed::GetCurrentZoom() * 100);
		SetCursorPosX(GetWindowWidth() - CalcTextSize("Modifiers").x - 15.0f);
		MenuItem("Modifiers");
		EndMenuBar();
	}

	if (mainMenuOpen) {
		BeginChild("NodeEditorLeftMenu", ImVec2(200.0f, 0.0f), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);
		
		if (Selectable(ICON_CI_ADD " New Node Graph")) {
			NodeGraph ng;
			ng.name = "Untitled Node Graph";
			song->nodeGraphs.push_back(ng);
		}

		SeparatorText("Node Graphs");
		
		int it = 0;
		for (auto& ng : song->nodeGraphs) {
			if (Selectable((ng.name + "##" + std::to_string(it)).c_str())) {
				openedNodeGraph = &ng;
				reloadNodeEditorContents();
			}
			it++;
		}

		EndChild();

		SameLine();
	}

	if (!openedNodeGraph) {
		const char txt[] = "NO NODE GRAPH OPENED";
		const ImVec2 txtSize = CalcTextSize(txt);
		const ImVec2 win = GetWindowSize();
		SetCursorPos(ImVec2((win.x - txtSize.x) / 2.0f, (win.y - txtSize.y) / 2.0f));
		TextUnformatted(txt);
		ed::SetCurrentEditor(nullptr);
		End();
		return;
	}

	ed::Begin("AXENodeEditor", ImVec2(0.0f, 0.0f));

	for (auto& node : openedNodeGraph->nodes) {
		PushID(node.id.AsPointer());
		ed::PushStyleColor(ed::StyleColor_NodeBg, node.color);
		ed::BeginNode(node.id);

		// ImDrawList* bgDrawList = ed::GetNodeBackgroundDrawList(node.id);
		// ImVec2 topLeft = GetCursorScreenPos();
		// bgDrawList->AddRectFilled(topLeft, ImVec2(topLeft.x + 1000.0f, topLeft.y + 10.0f), node.color);

		Text("%s", node.name.c_str());

		for (auto& input : node.inputs) {
			ed::BeginPin(input.id, ed::PinKind::Input);
				Text("%s", input.name.c_str());
			ed::EndPin();
		}

		for (auto& output : node.outputs) {
			SetCursorPosX(GetCursorPosX() + CalcTextSize(output.name.c_str()).x);
			ed::BeginPin(output.id, ed::PinKind::Output);
				Text("%s", output.name.c_str());
			ed::EndPin();
		}

		ed::EndNode();
		ed::PopStyleColor();
		PopID();
	}

	for (auto& linkInfo : openedNodeGraph->links)
		ed::Link(linkInfo.id, linkInfo.inputId, linkInfo.outputId);

	if (ed::BeginCreate()) { // Editor wants to create something...

		ed::PinId inputPinid, outputPinId;
		if (ed::QueryNewLink(&inputPinid, &outputPinId)) { // Editor wants to create new link...
			// Here is where we validate links
			if (inputPinid.Get() == outputPinId.Get()) {
				SetTooltip(ICON_CI_X "Pin can't connect to itself");
			}
			if (inputPinid && outputPinId) { // LGTM!
				// ed::RejectNewItem to say no
				// ed::RejectNewItem();
				if (ed::AcceptNewItem()) {
					openedNodeGraph->links.push_back({ ed::LinkId(nextLinkId++), inputPinid, outputPinId });
				}
			}
		}
	}
	ed::EndCreate();

	if (ed::BeginDelete()) {
		ed::LinkId deletedLinkId;
		while (ed::QueryDeletedLink(&deletedLinkId)) {
			if (ed::AcceptDeletedItem()) {
				for (auto& link : openedNodeGraph->links) {
					if (link.id == deletedLinkId) {
						openedNodeGraph->links.erase(&link);
						break;
					}
				}
			}
		}

		ed::NodeId deletedNodeId;
		while (ed::QueryDeletedNode(&deletedNodeId)) {
			if (ed::AcceptDeletedItem()) {
				int it = 0;
				for (auto& node : openedNodeGraph->nodes) {
					if (node.id == deletedNodeId) {
						openedNodeGraph->nodes.erase(openedNodeGraph->nodes.begin() + it);
						break;
					}
					it++;
				}
			}
		}
	}
	ed::EndDelete();

	if (ed::ShowNodeContextMenu(&contextNodeId)) {
		OpenPopup("Node Context Menu");
	} else if (ed::ShowLinkContextMenu(&contextLinkId)) {
		OpenPopup("Link Context Menu");
	} else if (ed::ShowBackgroundContextMenu()) {
		OpenPopup("Create New Node");
	}

#if 0
	ed::Suspend();
	if (BeginPopup("Create New Node")) {
		if (MenuItem("Low Pass Filter")) SpawnLowPassFilterNode();
		if (MenuItem("Delay / Echo")) SpawnDelayNode();
		EndPopup();
	}
	ed::Resume();
#endif

	ed::End();
	ed::SetCurrentEditor(nullptr);

	End();
}

void AXENodeEditor::reloadNodeEditorContents() {
	if (!openedNodeGraph) return;

	PPK_ASSERT(ed::GetCurrentEditor() != nullptr);

	ed::NavigateToContent();
}

void AXENodeEditor::shutdown() {
	ed::DestroyEditor(editorCtx);
}

void AXENodeEditor::SpawnLowPassFilterNode() {
	Node newNode;
	newNode.name = "Low Pass Filter";
	newNode.id = getNextId();
	newNode.color = ImColor(50, 0, 0, 255);

	Pin input;
	input.id = getNextId();
	input.name = "Audio In";
	
	Pin output;
	output.id = getNextId();
	output.name = "Audio Out";

	newNode.inputs.push_back(input);
	newNode.outputs.push_back(output);
	
	openedNodeGraph->nodes.push_back(newNode);
}

void AXENodeEditor::SpawnDelayNode() {
	Node newNode;
	newNode.name = "Delay / Echo";
	newNode.id = getNextId();
	newNode.color = ImColor(50, 0, 0, 255);

	Pin input;
	input.id = getNextId();
	input.name = "Audio In";
	
	Pin output;
	output.id = getNextId();
	output.name = "Audio Out";

	newNode.inputs.push_back(input);
	newNode.outputs.push_back(output);
	
	openedNodeGraph->nodes.push_back(newNode);
}

void AXENodeEditor::SpawnInputNode() {
	Node newNode;
	newNode.name = "Data Source Input";
	newNode.id = getNextId();
	newNode.color = ImColor(50, 0, 0, 255);
	
	Pin output;
	output.id = getNextId();
	output.name = "Audio Out";

	newNode.outputs.push_back(output);
	
	openedNodeGraph->nodes.push_back(newNode);
}

void AXENodeEditor::SpawnOutputNode() {
	Node newNode;
	newNode.name = "Output";
	newNode.id = getNextId();
	newNode.color = ImColor(50, 0, 0, 255);

	Pin input;
	input.id = getNextId();
	input.name = "Audio In";

	newNode.inputs.push_back(input);
	
	openedNodeGraph->nodes.push_back(newNode);
}

}