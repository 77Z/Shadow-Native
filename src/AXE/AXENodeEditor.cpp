#define IMGUI_DEFINE_MATH_OPERATORS
#include "AXENodeEditor.hpp"
#include "AXETypes.hpp"
#include "Debug/Logger.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "IconsCodicons.h"
#include "imgui/imgui_utils.hpp"
#include <memory>
#include <string>
#include "../imgui/imgui_knobs.hpp"
#include "ppk_assert_impl.hpp"

// Forward declarations
namespace Shadow::AXE {
bool compileNodeGraph(NodeGraph* graph);
}

namespace Shadow::AXE {

static ImRect ImRect_Expanded(const ImRect& rect, float x, float y) {
	ImRect result = rect;
	result.Min.x -= x;
	result.Min.y -= y;
	result.Max.x += x;
	result.Max.y += y;
	return result;
}

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

	ed::SetCurrentEditor(editorCtx);

	TextUnformatted("Currently opened node graph: ");
	PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
	Text("%s", openedNodeGraph->name.c_str());
	PopStyleColor();
	Text("Last known id: %i", openedNodeGraph->lastKnownGraphId);
	
	if (SmallButton("Show Graph Flow"))
		for (auto& link: openedNodeGraph->links)
			ed::Flow(link.id);

	SeparatorText("Links:");
	for (auto& link : openedNodeGraph->links) {
		Bullet();
		Text("Link #%lu - Connects %lu to %lu", link.id.Get(), link.inputId.Get(), link.outputId.Get());
	}

	SeparatorText("Nodes:");
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

	SeparatorText("Selection");

	if (SmallButton("Clear Selection")) ed::ClearSelection();

	for (int i = 0; i < selectedNodeCount; ++i) Text("Node (%p)", selectedNodes[i].AsPointer());
	for (int i = 0; i < selectedLinkCount; ++i) Text("Link (%p)", selectedLinks[i].AsPointer());


	ed::SetCurrentEditor(nullptr);	

	End();
}

void AXENodeEditor::nodeAddMenu() {
	using namespace ImGui;

	SeparatorText("Essential");
	if (MenuItem("Input", "I")) SpawnInputNode();
	if (MenuItem("Output", "O")) SpawnOutputNode();
	SeparatorText("Filtering");
	MenuItem("Biquad");
	if (MenuItem("Low Pass")) SpawnLowPassFilterNode();
	MenuItem("High Pass");
	MenuItem("Low Shelf");
	MenuItem("High Shelf");
	MenuItem("Band Pass");
	MenuItem("Notch");
	MenuItem("Peaking EQ");
	SeparatorText("Effects");
	if (MenuItem("Delay / Echo")) SpawnDelayNode();
	SeparatorText("Generation");
	MenuItem("Waveform");
	MenuItem("Noise");
	Separator();
	if (MenuItem("Comment", "C")) SpawnComment();
}

void AXENodeEditor::onUpdate(bool& p_open) {
	using namespace ImGui;

	if (!p_open) return;

	Begin("Node Editor", &p_open, ImGuiWindowFlags_MenuBar);
	ed::SetCurrentEditor(editorCtx);

	ImGuiIO io = GetIO();
	if (IsWindowHovered()
		&& io.KeyCtrl
		&& IsKeyPressed(GetKeyIndex(ImGuiKey_B))
		&& openedNodeGraph) {
		compileNodeGraph(openedNodeGraph);
		for (auto& link: openedNodeGraph->links)
			ed::Flow(link.id);
	}

	if (IsWindowHovered() && openedNodeGraph && !io.WantTextInput) {
		if (IsKeyPressed(ImGuiKey_I)) SpawnInputNode();
		if (IsKeyPressed(ImGuiKey_O)) SpawnOutputNode();
		if (IsKeyPressed(ImGuiKey_C)) SpawnComment();
	}

	// update selection
	if (openedNodeGraph) {
		selectedNodes.resize(ed::GetSelectedObjectCount());
		selectedLinks.resize(ed::GetSelectedObjectCount());
		selectedNodeCount = ed::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
		selectedLinkCount = ed::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));
		// selectedNodes.resize(nodeCount);
		// selectedLinks.resize(linkCount);
	}

	if (BeginMenuBar()) {
		if (MenuItem(ICON_CI_MENU " Graphs")) mainMenuOpen = !mainMenuOpen;
		BeginDisabled(!openedNodeGraph);
		if (BeginMenu("+ Add")) {
			nodeAddMenu();
			EndMenu();
		}
		if (BeginMenu("Graph")) {
			if (MenuItem("Compile", "CTRL + B")) {
				compileNodeGraph(openedNodeGraph);
				for (auto& link: openedNodeGraph->links)
					ed::Flow(link.id);
			}
			if (MenuItem("Close"))
				openedNodeGraph = nullptr;
			EndMenu();
		}
		if (BeginMenu("View")) {
			if (MenuItem("Recenter", "F")) ed::NavigateToContent();
			EndMenu();
		}
		EndDisabled();
		if (openedNodeGraph) {
			TextUnformatted("|");
			PushItemWidth(300.0f);
			PushStyleColor(ImGuiCol_FrameBg, GetStyle().Colors[ImGuiCol_MenuBarBg]);
			InputText("##nodegraphname", &openedNodeGraph->name);
			PopStyleColor();

			Text("| %.0f%%", ed::GetCurrentScale() * 100);

			TextUnformatted("|");
			if (openedNodeGraph->lastModified > openedNodeGraph->lastCompiled) {
				TextUnformatted(ICON_CI_WARNING " Out of date");
			} else {
				TextUnformatted(ICON_CI_CHECK " Up to date");
			}
			if (BeginItemTooltip()) {
				PushTextWrapPos(GetFontSize() * 35.0f);
				TextUnformatted(
					"Out of date node graphs need to be compiled before use in your project. This "
					"happens automatically when you press play, but compilation could take some "
					"time for larger graphs, which could add some jarring latency to what you'd "
					"expect from AXE.\n\n"
					"As yet another measure to let you go crazy with AXE, you can trigger node "
					"graph compilations at any time using the menu, or by pressing CTRL + B");
				PopTextWrapPos();
				EndTooltip();
			}
		}

		SetCursorPosX(GetWindowWidth() - CalcTextSize("Modifiers").x - 15.0f);
		MenuItem("Modifiers");
		EndMenuBar();
	}

	if (mainMenuOpen) {
		BeginChild("NodeEditorLeftMenu", ImVec2(200.0f, 0.0f), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);

		if (Selectable(ICON_CI_ADD " New Node Graph")) {
			createNodeGraph("Untitled Node Graph");
		}

		if (Selectable(ICON_CI_GRAPH " Open Master Graph")) {
			//
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

	for (auto& comment : openedNodeGraph->comments) {
		PushStyleVar(ImGuiStyleVar_Alpha, 0.75f);
		ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
		ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
		ed::BeginNode(comment.id);
		PushID(comment.id.AsPointer());

		// TextUnformatted(comment.data.c_str());
		InputText("##commentData", &comment.data);

		ed::Group(comment.size);

		PopID();
		ed::EndNode();
		PopStyleVar();

		if (ed::BeginGroupHint(comment.id)) {
			
			int bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);
			ImVec2 min = ed::GetGroupMin();

			SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
			BeginGroup();
			TextUnformatted(comment.data.c_str());
			EndGroup();

			ImDrawList* drawList = ed::GetHintBackgroundDrawList();

			ImRect hintBounds      = ImRect(GetItemRectMin(), GetItemRectMax());
			ImRect hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

			drawList->AddRectFilled(
				hintFrameBounds.GetTL(),
				hintFrameBounds.GetBR(),
				IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

			drawList->AddRect(
				hintFrameBounds.GetTL(),
				hintFrameBounds.GetBR(),
				IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

			ed::EndGroupHint();
		}
	}

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

		for (auto& prop: node.props) {
			switch (prop.propType) {
			case NodeProperties_S8:
			case NodeProperties_U8:
			case NodeProperties_S16:
			case NodeProperties_U16:
				break;
			case NodeProperties_S32: {
				std::shared_ptr<int> minPtr = std::static_pointer_cast<int>(prop.min);
				std::shared_ptr<int> maxPtr = std::static_pointer_cast<int>(prop.max);
				ImGuiKnobs::KnobInt(
					prop.propLabel.c_str(),
					static_cast<int*>(prop.data.get()),
					*minPtr,
					*maxPtr);
				break;
			}
			case NodeProperties_U32:
			case NodeProperties_S64:
			case NodeProperties_U64:
				break;
			case NodeProperties_Float: {
				std::shared_ptr<float> minPtr = std::static_pointer_cast<float>(prop.min);
				std::shared_ptr<float> maxPtr = std::static_pointer_cast<float>(prop.max);
				ImGuiKnobs::Knob(
					prop.propLabel.c_str(),
					static_cast<float*>(prop.data.get()),
					*minPtr,
					*maxPtr);
				break;
			}
			case NodeProperties_Double: {
				std::shared_ptr<double> minPtr = std::static_pointer_cast<double>(prop.min);
				std::shared_ptr<double> maxPtr = std::static_pointer_cast<double>(prop.max);
				ImGuiKnobs::Knob(
					prop.propLabel.c_str(),
					static_cast<float*>(prop.data.get()),
					*minPtr,
					*maxPtr);
				break;
			}
			case NodeProperties_Bool:
			case NodeProperties_COUNT:
				break;
			}
			if (!prop.help.empty()) {
				SameLine();
				ImGui::TextDisabled("?");
				if (IsItemHovered()) helpText = prop.help;
			}
			SameLine();
		}
		InvisibleButton("##SamelineStop", ImVec2(1,1));

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
					openedNodeGraph->links.push_back({ ed::LinkId(getNextId()), inputPinid, outputPinId });
					markGraphDirty(openedNodeGraph);
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
						markGraphDirty(openedNodeGraph);
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
						markGraphDirty(openedNodeGraph);
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

	ed::Suspend();
	SetNextWindowPos(GetMousePos(), ImGuiCond_Appearing);
	if (BeginPopup("Create New Node")) {
		nodeAddMenu();
		EndPopup();
	}
	ed::Resume();

	ed::Suspend();
	SetNextWindowPos(GetMousePos(), ImGuiCond_Appearing);
	if (BeginPopup("Node Context Menu")) {
		Text("%i node%s selected", selectedNodeCount, selectedNodeCount == 1 ? "" : "s");
		Separator();
		if (MenuItem("Delete")) {
			for (int i = 0; i < selectedNodeCount; i++) ed::DeleteNode(selectedNodes[i]);
			for (int i = 0; i < selectedLinkCount; i++) ed::DeleteLink(selectedLinks[i]);
		}

		if (MenuItem(ICON_CI_GROUP_BY_REF_TYPE " Ship selection to new graph")) {
			// CRASHES
			// Needs to be reimplemented with new createNodeGraph method
#if 0
			NodeGraph shippedGraph;
			shippedGraph.name = "Shipped Graph";
			std::time_t t = std::time(nullptr);
			shippedGraph.lastModified = t;
			shippedGraph.lastCompiled = t;

			for (int i = 0; i < selectedNodeCount; i++) {
				// if (!openedNodeGraph) break;
				for (auto& node : openedNodeGraph->nodes) {
					if (node.id == selectedNodes[i]) {
						PRINT("NODES MATCH");
						shippedGraph.nodes.push_back(node);
						ed::DeleteNode(selectedNodes[i]);
					}
				}
			}

			openedNodeGraph = nullptr;
			song->nodeGraphs.push_back(shippedGraph);
#endif
		}

		EndPopup();
	}
	ed::Resume();

	ed::End();
	ed::SetCurrentEditor(nullptr);

	if (!helpText.empty()) {
		float wrapX = GetFontSize() * 35.0f;
		ImVec2 textSize = CalcTextSize(helpText.c_str(), nullptr, false, wrapX);
		RenderTextWrapped(GetWindowPos() + GetWindowSize() - textSize - ImVec2(10,10), helpText.c_str(), nullptr, wrapX);
		GetForegroundDrawList()->AddRect(GetWindowPos(), GetWindowPos() + GetWindowSize(), IM_COL32(255, 0, 0, 255));
		helpText = "";
	}

	End();
}

void AXENodeEditor::reloadNodeEditorContents() {
#if 0
	if (!openedNodeGraph) return;

	PPK_ASSERT(ed::GetCurrentEditor() != nullptr);

	ed::NavigateToContent();
#endif
}

void AXENodeEditor::shutdown() {
	ed::DestroyEditor(editorCtx);
}

void AXENodeEditor::markGraphDirty(NodeGraph* graph) {
	if (!graph) return;
	graph->lastModified = std::time(nullptr);
}

void AXENodeEditor::createNodeGraph(const std::string& name) {
	openedNodeGraph = nullptr;
	NodeGraph ng;
	// ng.ctx = std::make_shared<ed::EditorContext>(ed::CreateEditor(&defaultConfig));
	ng.name = name;
	std::time_t t = std::time(nullptr);
	ng.lastModified = t;
	ng.lastCompiled = t;

#if 0
	ed::SetCurrentEditor(editorCtx);

	auto& colors = ed::GetStyle().Colors;
	colors[ed::StyleColor_Bg] = ImColor(0, 0, 0, 255);
	colors[ed::StyleColor_HovNodeBorder] = ImColor(255, 69, 0, 255);
	colors[ed::StyleColor_SelNodeBorder] = ImColor(255, 0, 0, 255);
	colors[ed::StyleColor_PinRect] = ImColor(255, 0, 0, 255);

	ed::SetCurrentEditor(nullptr);
#endif

	song->nodeGraphs.push_back(ng);
}

// template<typename T>
// static void constructProps(T f) {}

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

	NodeProperty cutoffFreqProp;
	cutoffFreqProp.propLabel = "Cutoff Factor";
	cutoffFreqProp.propType = NodeProperties_S32;
	cutoffFreqProp.data = std::make_shared<int>(80);
	cutoffFreqProp.min = std::make_shared<int>(0);
	cutoffFreqProp.max = std::make_shared<int>(100);
	cutoffFreqProp.size = sizeof(int);
	cutoffFreqProp.help = "A number divided by the sample rate to get a cutoff frequency";
	newNode.props.push_back(cutoffFreqProp);

	NodeProperty filterOrderProp;
	filterOrderProp.propLabel = "Filter Order";
	filterOrderProp.propType = NodeProperties_S32;
	filterOrderProp.data = std::make_shared<int>(8);
	filterOrderProp.min = std::make_shared<int>(0);
	filterOrderProp.max = std::make_shared<int>(60);
	filterOrderProp.size = sizeof(int);
	filterOrderProp.help = "Controls the complexity of the filter. Filters with more complexity "
						   "have sharper frequency responses, useful when trying to seperate "
						   "frequencies that are close to each other. Higher orders require more "
						   "processing power";
	newNode.props.push_back(filterOrderProp);
	
	openedNodeGraph->nodes.push_back(newNode);
	markGraphDirty(openedNodeGraph);
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

	NodeProperty delayProp;
	delayProp.propLabel = "Delay (sec)";
	delayProp.propType = NodeProperties_Float;
	delayProp.data = std::make_shared<float>(0.2f);
	delayProp.min = std::make_shared<float>(0.0f);
	delayProp.max = std::make_shared<float>(60.0f);
	delayProp.size = sizeof(float);
	newNode.props.push_back(delayProp);

	NodeProperty decayProp;
	decayProp.propLabel = "Decay";
	decayProp.propType = NodeProperties_Float;
	decayProp.data = std::make_shared<float>(0.5f);
	decayProp.min = std::make_shared<float>(0.0f);
	decayProp.max = std::make_shared<float>(1.0f);
	decayProp.size = sizeof(float);
	decayProp.help = "Decay controls the volume falloff per each echo";
	newNode.props.push_back(decayProp);
	
	openedNodeGraph->nodes.push_back(newNode);
	markGraphDirty(openedNodeGraph);
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
	markGraphDirty(openedNodeGraph);
}

void AXENodeEditor::SpawnOutputNode() {
	Node newNode;
	newNode.name = "Output";
	newNode.id = getNextId();
	newNode.color = ImColor(50, 0, 0, 255);
	newNode.type = NodeType_Output;

	Pin input;
	input.id = getNextId();
	input.name = "Audio In";

	newNode.inputs.push_back(input);
	
	openedNodeGraph->nodes.push_back(newNode);
	markGraphDirty(openedNodeGraph);
}

void AXENodeEditor::SpawnComment() {
	NGComment c;

	c.id = getNextId();

	openedNodeGraph->comments.push_back(c);
}

}