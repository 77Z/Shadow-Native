#ifndef SHADOW_NATIVE_AXE_AXE_NODE_EDITOR_HPP
#define SHADOW_NATIVE_AXE_AXE_NODE_EDITOR_HPP

#include "../nodeEditor/imgui_node_editor.h"
#include "imgui.h"
#include <string>
#include <vector>

namespace Shadow::AXE {

namespace ed = ax::NodeEditor;

struct Node;

enum PinType_ {};

enum PinKind_ {
	PinKind_Input,
	PinKind_Output,
	PinKind_COUNT,
};

struct Pin {
	ed::PinId id;
	Node* node;
	std::string name;
	PinType_ type;
	PinKind_ kind;
};

struct Link {
	ed::LinkId id;
	ed::PinId inputId;
	ed::PinId outputId;
};

enum NodeType_ {
	NodeType_Regular,
};

struct Node {
	ed::NodeId id;
	std::string name;
	std::vector<Pin> inputs;
	std::vector<Pin> outputs;
	ImColor color;
	NodeType_ type;
	ImVec2 size;
};

class AXENodeEditor {
public:
	AXENodeEditor();
	~AXENodeEditor();

	void onUpdate(bool& p_open);

	void updateDebugMenu(bool& p_open);

	void shutdown();


private:
	ed::EditorContext* editorCtx;
	ImVector<Link> links;
	std::vector<Node> nodes;
	int nextLinkId = 1000;

	// Temporary container to take actions on things from ctx menus
	ed::NodeId contextNodeId;
	ed::LinkId contextLinkId;

	int getNextId() { return nextLinkId++; }
	// int getNextLinkId() { return ed::LinkId(getNextId()); }

	void SpawnInputNode();
	void SpawnOutputNode();
	void SpawnLowPassFilterNode();
	void SpawnDelayNode();
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_NODE_EDITOR_HPP */