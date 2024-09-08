#ifndef SHADOW_NATIVE_AXE_AXE_NODE_EDITOR_HPP
#define SHADOW_NATIVE_AXE_AXE_NODE_EDITOR_HPP

#include "../nodeEditor/imgui_node_editor.h"
#include "AXETypes.hpp"

namespace Shadow::AXE {

namespace ed = ax::NodeEditor;

class AXENodeEditor {
public:
	AXENodeEditor(Song* song);
	~AXENodeEditor();

	void onUpdate(bool& p_open);

	void updateDebugMenu(bool& p_open);

	void shutdown();


private:
	Song* song;
	NodeGraph* openedNodeGraph = nullptr;
	ed::EditorContext* editorCtx;
	// int nextLinkId = 1000;

	bool mainMenuOpen = true;

	// Temporary container to take actions on things from ctx menus
	ed::NodeId contextNodeId;
	ed::LinkId contextLinkId;

	std::vector<ed::NodeId> selectedNodes;
	std::vector<ed::LinkId> selectedLinks;
	int selectedNodeCount = 0;
	int selectedLinkCount = 0;

	// int getNextId() { return nextLinkId++; }
	int getNextId() { return openedNodeGraph->lastKnownGraphId++; }
	// int getNextLinkId() { return ed::LinkId(getNextId()); }

	void markGraphDirty(NodeGraph* graph);

	// Really a terrible name on my part. When a new node graph is loaded into
	// `openedNodeGraph`, the contents of the node editor need to reflect this.
	// This method is for just that.
	void reloadNodeEditorContents();


	void nodeAddMenu();

	void SpawnInputNode();
	void SpawnOutputNode();
	void SpawnLowPassFilterNode();
	void SpawnDelayNode();
	void SpawnComment();

	void createNodeGraph(const std::string& name);

	std::string helpText;
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_NODE_EDITOR_HPP */