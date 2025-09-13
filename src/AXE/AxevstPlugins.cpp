//
// Created by vince on 9/12/25.
//

#include "AxevstPlugins.hpp"

#include "imgui.h"

namespace Shadow::AXE {

AXEVSTPlugins::AXEVSTPlugins() {  }

AXEVSTPlugins::~AXEVSTPlugins() {  }

void AXEVSTPlugins::onUpdate(bool& p_open) {
	using namespace ImGui;

	if (!p_open) return;

	if (!Begin("VST Plugins", &p_open)) {
		End();
		return;
	}

	SeparatorText("Available Plugins:");

	End();
}


}