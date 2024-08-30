#include "AXEEqualizer.hpp"
#include "imgui.h"
#include "../implot/implot.h"
#include <cmath>

namespace Shadow::AXE {

AXEEqualizer::AXEEqualizer() {};

AXEEqualizer::~AXEEqualizer() {};

void AXEEqualizer::onUpdate(bool& p_open) {
	using namespace ImGui;

	if (!p_open) return;

	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	if (!Begin("Visual Equalizer", &p_open, ImGuiWindowFlags_NoCollapse)) {
		End();
		PopStyleVar();
		return;
	}

	static double x[20001], y[20001];
	for (int i = 0; i < 20001; ++i) {
		x[i]  = i*0.1f;
		y[i] = std::sin(x[i]) + 1;
	}

	if (ImPlot::BeginPlot("Visual Equalizer", ImVec2(-1,-1), ImPlotFlags_NoTitle)) {
		ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
		ImPlot::SetupAxesLimits(0.1, 20000, -12, 12);
		ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0.1, 20000);
		ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, -12, 12);
		ImPlot::SetupAxisFormat(ImAxis_Y1, "%g dB");
		ImPlot::SetupAxisFormat(ImAxis_X1, "%g Hz");
		
		ImPlot::PlotLine("audio",     x, y, 21);

		ImPlot::EndPlot();
	}

	End();
	PopStyleVar();
}

}