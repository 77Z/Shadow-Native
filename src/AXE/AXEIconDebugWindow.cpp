#include "imgui.h"

// Forward Decls.
namespace Shadow::AXE {
void iconDebugDrawingDrawAllIcons();
}

namespace Shadow::AXE {

void updateIconDebugWindow(bool& p_open) {
	using namespace ImGui;

	if (!p_open) return;

	if (!Begin("Icon Debug Window", &p_open)) {
		End();
		return;
	}

	iconDebugDrawingDrawAllIcons();

	End();
}

}