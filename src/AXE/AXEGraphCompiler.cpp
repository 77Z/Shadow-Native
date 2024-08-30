#include "AXETypes.hpp"
#include "ImGuiNotify.hpp"

namespace Shadow::AXE {

static void failCompile(const char* msg) {
	ImGui::InsertNotification({
		ImGuiToastType::Error,
		5000,
		"NODE GRAPH COMPILATION ERROR\n%s", msg
	});
}

bool compileNodeGraph(NodeGraph* graph) {

	if (graph == nullptr) {
		failCompile("No node graph open to compile!");
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
		failCompile("No output node speicified!\nThis audio has nowhere to go!");
		return false;
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