#include "AXETypes.hpp"
#include "Debug/EditorConsole.hpp"
#include "ImGuiNotify.hpp"

#define EC_THIS "GraphCompiler"

namespace Shadow::AXE {

static ma_result result;

static void failCompile(const char* msg) {
	ImGui::InsertNotification({
		ImGuiToastType::Error,
		5000,
		"NODE GRAPH COMPILATION ERROR\n%s", msg
	});
}

#if 0
// Searches through all links to find the node that is connected
// to the input pin id provided.
// return type is a Node*, but can be nullptr if no node is connected to your
// provided input pin.
static Node* findPrevNode(NodeGraph* graph, ed::NodeId knownId) {
	// What a terrible search alg ladies and gentlemen
	for (auto& link : graph->links) {
		if (link.outputId.Get() == knownId.Get()) {
			for (auto& node : graph->nodes) {
				for (auto& outpin : node.outputs) {
					if (link.inputId.Get() == outpin.id.Get()) {
						// this is the node
						return &node;
					}
				}
			}
			if (link.id.Get())
			break;
		}
	}
	return nullptr;
}
#endif

static Node* findPrevNode(NodeGraph* graph, ed::NodeId knownId) {
	// Create a hash map to map input pin IDs to their corresponding nodes
	std::unordered_map<uint64_t, Node*> pinToNodeMap;

	// Preprocess the nodes and their output pins
	for (auto& node : graph->nodes) {
		for (auto& outpin : node.outputs) {
			pinToNodeMap[outpin.id.Get()] = &node;
		}
	}

	// Search for the node connected to the given input pin ID
	for (auto& link : graph->links) {
		if (link.outputId.Get() == knownId.Get()) {
			auto it = pinToNodeMap.find(link.inputId.Get());
			if (it != pinToNodeMap.end()) {
				return it->second; // Return the connected node
			}
		}
	}

	return nullptr; // No connected node found
}

bool compileNodeGraph(NodeGraph* graph) {

	EC_NEWCAT(EC_THIS);

	if (graph == nullptr) {
		failCompile("No node graph open to compile!");
		return false;
	}

	EC_PRINT(EC_THIS, "Compiling node graph: %s", graph->name.c_str());

	ma_node_graph_config graphCfg = ma_node_graph_config_init(2);
	// SHOULD THIS BE HEAP ALLOCATED WITH SMART POINTERS???
	result = ma_node_graph_init(&graphCfg, nullptr, &graph->compiledGraph);
	if (result != MA_SUCCESS) {
		failCompile("Couldn't init internal engine graph :/");
		return false;
	}

	

	// Search for output node
	Node* outputNode = nullptr;
	for (auto& node : graph->nodes) {
		if (node.type == NodeType_Output) {
			outputNode = &node;
		}
	}
	if (!outputNode) {
		// No output!
		failCompile("No output node specified!\nThis audio has nowhere to go!");
		return false;
	}

	EC_PRINT(EC_THIS, "Output node id: %lu", outputNode->id.Get());
	EC_PRINT(EC_THIS, "Output node's input pin id: %lu", outputNode->inputs[0].id.Get());

	EC_PRINT(EC_THIS, "connected to output node:");
	Node* prevNode = findPrevNode(graph, outputNode->inputs[0].id.Get());
	if (prevNode == nullptr) {
		EC_PRINT(EC_THIS, "No node connected");
		ImGui::InsertNotification({
			ImGuiToastType::Warning,
			5000,
			"WARNING\nNothing connected to output node,\nthis graph will be silent!"
		});
	} else {
		EC_PRINT(EC_THIS, "Node: %s", prevNode->name.c_str());
	}

	ImGui::InsertNotification({
		ImGuiToastType::Success,
		3000,
		"Node graph compiled!"
	});

	graph->lastCompiled = std::time(nullptr);

	return true;
}

}