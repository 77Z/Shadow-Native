#ifndef SHADOW_NATIVE_AXE_AXE_NODE_EDITOR_HPP
#define SHADOW_NATIVE_AXE_AXE_NODE_EDITOR_HPP

#include "AXETypes.hpp"

namespace Shadow::AXE {

namespace ed = ax::NodeEditor;

class AXENodeEditor {
public:
	AXENodeEditor(Song* song, EditorState* editorState);
	~AXENodeEditor();

	void onUpdate(bool& p_open);

	void updateDebugMenu(bool& p_open);

	void shutdown();

	// void getMasterNodeGraph();


private:
	Song* song;
	EditorState* editorState;
	NodeGraph* openedNodeGraph = nullptr;

	bool mainMenuOpen = true;

	// Temporary container to take actions on things from ctx menus
	ed::NodeId contextNodeId;
	ed::LinkId contextLinkId;

	std::vector<ed::NodeId> selectedNodes;
	std::vector<ed::LinkId> selectedLinks;
	int selectedNodeCount = 0;
	int selectedLinkCount = 0;

	int getNextId() { return song->lastKnownGraphId++; }
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

	NodeGraph* createNodeGraph(const std::string& name);

	std::string helpText;
};

}

#endif /* SHADOW_NATIVE_AXE_AXE_NODE_EDITOR_HPP */