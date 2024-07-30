#include "AXENodeEditor.hpp"
#include "imgui.h"

namespace Shadow::AXE {

AXENodeEditor::AXENodeEditor() {
	// Save settings inside axe file in future
	ed::Config config;
	config.SettingsFile = "nodeeditor.json";
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
	if (!p_open) return;

	ImGui::Begin("Node Editor Diagnostics", &p_open);

	ImGui::Text("Currently opened node graph: ");

	ImGui::TextUnformatted("Links:");
	for (auto& link : links) {
		ImGui::Bullet();
		ImGui::Text("Link #%lu - Connects %lu to %lu", link.id.Get(), link.inputId.Get(), link.outputId.Get());
	}

	ImGui::TextUnformatted("Nodes:");
	for (auto& node : nodes) {
		ImGui::Bullet();
		ImGui::Text("Node #%lu, %s", node.id.Get(), node.name.c_str());
		for (auto& pin : node.inputs) {
			ImGui::Indent();
			ImGui::Text("Input pin #%lu", pin.id.Get());
			ImGui::Unindent();
		}
		for (auto& pin : node.outputs) {
			ImGui::Indent();
			ImGui::Text("Output pin #%lu", pin.id.Get());
			ImGui::Unindent();
		}
	}

	ImGui::End();
}

void AXENodeEditor::onUpdate(bool& p_open) {
	if (!p_open) return;

	ImGui::Begin("Node Editor", &p_open, ImGuiWindowFlags_MenuBar);
	ed::SetCurrentEditor(editorCtx);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("+ Add")) {
			ImGui::SeparatorText("Essential");
			if (ImGui::MenuItem("Input")) SpawnInputNode();
			if (ImGui::MenuItem("Output")) SpawnOutputNode();
			ImGui::SeparatorText("Effects");
			if (ImGui::MenuItem("Low Pass Filter")) SpawnLowPassFilterNode();
			if (ImGui::MenuItem("Delay / Echo")) SpawnDelayNode();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Position")) {
			if (ImGui::MenuItem("Recenter", "F")) ed::NavigateToContent();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ed::Begin("AXENodeEditor", ImVec2(0.0f, 0.0f));

	for (auto& node : nodes) {
		ImGui::PushID(node.id.AsPointer());
		ed::PushStyleColor(ed::StyleColor_NodeBg, node.color);
		ed::BeginNode(node.id);

		// ImDrawList* bgDrawList = ed::GetNodeBackgroundDrawList(node.id);
		// ImVec2 topLeft = ImGui::GetCursorScreenPos();
		// bgDrawList->AddRectFilled(topLeft, ImVec2(topLeft.x + 1000.0f, topLeft.y + 10.0f), node.color);

		ImGui::Text("%s", node.name.c_str());

		for (auto& input : node.inputs) {
			ed::BeginPin(input.id, ed::PinKind::Input);
				ImGui::Text("%s", input.name.c_str());
			ed::EndPin();
		}

		for (auto& output : node.outputs) {
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::CalcTextSize(output.name.c_str()).x);
			ed::BeginPin(output.id, ed::PinKind::Output);
				ImGui::Text("%s", output.name.c_str());
			ed::EndPin();
		}

		ed::EndNode();
		ed::PopStyleColor();
		ImGui::PopID();
	}

	for (auto& linkInfo : links)
		ed::Link(linkInfo.id, linkInfo.inputId, linkInfo.outputId);

	if (ed::BeginCreate()) { // Editor wants to create something...

		ed::PinId inputPinid, outputPinId;
		if (ed::QueryNewLink(&inputPinid, &outputPinId)) { // Editor wants to create new link...
			// Here is where we validate links
			if (inputPinid && outputPinId) { // LGTM!
				// ed::RejectNewItem to say no
				if (ed::AcceptNewItem()) {
					links.push_back({ ed::LinkId(nextLinkId++), inputPinid, outputPinId });
				}
			}
		}
	}
	ed::EndCreate();

	if (ed::BeginDelete()) {
		ed::LinkId deletedLinkId;
		while (ed::QueryDeletedLink(&deletedLinkId)) {
			if (ed::AcceptDeletedItem()) {
				for (auto& link : links) {
					if (link.id == deletedLinkId) {
						links.erase(&link);
						break;
					}
				}
			}
		}

		ed::NodeId deletedNodeId;
		while (ed::QueryDeletedNode(&deletedNodeId)) {
			if (ed::AcceptDeletedItem()) {
				int it = 0;
				for (auto& node : nodes) {
					if (node.id == deletedNodeId) {
						nodes.erase(nodes.begin() + it);
						break;
					}
					it++;
				}
			}
		}
	}
	ed::EndDelete();

	if (ed::ShowNodeContextMenu(&contextNodeId)) {
		ImGui::OpenPopup("Node Context Menu");
	} else if (ed::ShowLinkContextMenu(&contextLinkId)) {
		ImGui::OpenPopup("Link Context Menu");
	} else if (ed::ShowBackgroundContextMenu()) {
		ImGui::OpenPopup("Create New Node");
	}

#if 0
	ed::Suspend();
	if (ImGui::BeginPopup("Create New Node")) {
		if (ImGui::MenuItem("Low Pass Filter")) SpawnLowPassFilterNode();
		if (ImGui::MenuItem("Delay / Echo")) SpawnDelayNode();
		ImGui::EndPopup();
	}
	ed::Resume();
#endif

	ed::End();
	ed::SetCurrentEditor(nullptr);

	ImGui::End();
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
	
	nodes.push_back(newNode);
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
	
	nodes.push_back(newNode);
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
	
	nodes.push_back(newNode);
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
	
	nodes.push_back(newNode);
}

}