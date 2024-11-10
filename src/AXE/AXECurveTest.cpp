#include "AXETypes.hpp"
#include "Debug/Logger.hpp"
#include "imgui/imgui_utils.hpp"
#include "json_impl.hpp"
#include <string>
#include <vector>
#define IMGUI_DEFINE_MATH_OPERATORS
// #include "ImCurveEdit.h"
#include "imgui.h"

namespace Shadow::AXE {
/* 
struct RampEdit : public ImCurveEdit::Delegate {
	RampEdit() {
		mPts[0][0] = ImVec2(-10.f, 0);
		mPts[0][1] = ImVec2(20.f, 0.6f);
		mPts[0][2] = ImVec2(25.f, 0.2f);
		mPts[0][3] = ImVec2(70.f, 0.4f);
		mPts[0][4] = ImVec2(120.f, 1.f);
		mPointCount[0] = 5;

		mPts[1][0] = ImVec2(-50.f, 0.2f);
		mPts[1][1] = ImVec2(33.f, 0.7f);
		mPts[1][2] = ImVec2(80.f, 0.2f);
		mPts[1][3] = ImVec2(82.f, 0.8f);
		mPointCount[1] = 4;

		mPts[2][0] = ImVec2(40.f, 0);
		mPts[2][1] = ImVec2(60.f, 0.1f);
		mPts[2][2] = ImVec2(90.f, 0.82f);
		mPts[2][3] = ImVec2(150.f, 0.24f);
		mPts[2][4] = ImVec2(200.f, 0.34f);
		mPts[2][5] = ImVec2(250.f, 0.12f);
		mPointCount[2] = 6;
		mbVisible[0] = mbVisible[1] = mbVisible[2] = true;
		mMax = ImVec2(1.f, 1.f);
		mMin = ImVec2(0.f, 0.f);
	}
	size_t GetCurveCount() { return 3; }

	bool IsVisible(size_t curveIndex) { return mbVisible[curveIndex]; }
	size_t GetPointCount(size_t curveIndex) { return mPointCount[curveIndex]; }

	uint32_t GetCurveColor(size_t curveIndex) {
		uint32_t cols[] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };
		return cols[curveIndex];
	}
	ImVec2* GetPoints(size_t curveIndex) { return mPts[curveIndex]; }
	virtual ImCurveEdit::CurveType GetCurveType(size_t curveIndex) const {
		return ImCurveEdit::CurveSmooth;
	}
	virtual int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value) {
		mPts[curveIndex][pointIndex] = ImVec2(value.x, value.y);
		SortValues(curveIndex);
		for (size_t i = 0; i < GetPointCount(curveIndex); i++) {
			if (mPts[curveIndex][i].x == value.x)
				return (int)i;
		}
		return pointIndex;
	}
	virtual void AddPoint(size_t curveIndex, ImVec2 value) {
		if (mPointCount[curveIndex] >= 8)
			return;
		mPts[curveIndex][mPointCount[curveIndex]++] = value;
		SortValues(curveIndex);
	}
	virtual ImVec2& GetMax() { return mMax; }
	virtual ImVec2& GetMin() { return mMin; }
	virtual unsigned int GetBackgroundColor() { return 0; }
	ImVec2 mPts[3][8];
	size_t mPointCount[3];
	bool mbVisible[3];
	ImVec2 mMin;
	ImVec2 mMax;
private:
	void SortValues(size_t curveIndex) {
		auto b = std::begin(mPts[curveIndex]);
		auto e = std::begin(mPts[curveIndex]) + GetPointCount(curveIndex);
		std::sort(b, e, [](ImVec2 a, ImVec2 b) { return a.x < b.x; });

	}
}; */

// static RampEdit rampEdit;

static std::vector<ImVec2> points;
#if 0
void drawAutomation(ImRect bounds) {
	using namespace ImGui;

	if (GetCurrentWindow()->SkipItems) return;

	ImU32 defCol = IM_COL32(255, 0, 0, 255);

	ImDrawList* fg = GetForegroundDrawList();
	ImDrawList* drawList = GetWindowDrawList();
	fg->AddRect(bounds.Min, bounds.Max, IM_COL32(255, 0, 0, 255), 2.0f, 0, 2.0f);

	PushClipRect(bounds.Min, bounds.Max, true);

	int pointIt = 1;
	// !! TRIANGLE LEAK !! //
	// for (auto& point : points) {
	// 	ImVec2 screenPoint = bounds.Min + point;

	// 	bool hovered;
	// 	bool held;
	// 	bool press = ButtonBehavior(ImRect(screenPoint.x - 10, screenPoint.y - 10, screenPoint.x + 10, screenPoint.y + 10), GetID(pointIt), &hovered, &held);

	// 	if (hovered) defCol = IM_COL32(0, 255, 0, 255);

	// 	if (held) {
	// 		point = GetMousePos() - bounds.Min;
	// 	}

	// 	drawList->AddCircleFilled(bounds.Min + point, 10.0f, defCol, 4);
	// 	drawList->AddCircleFilled(bounds.Min + point, 3.0f, IM_COL32(0, 0, 0, 255), 4);

	// 	pointIt++;
	// }

	PopClipRect();
}
#endif

static ma_result result;
static ma_engine engine;

static ma_sound sound;

static void chkRes(ma_result res) {
	if (res != MA_SUCCESS) {
		ERROUT("ERR IN AUDIO ENGINE %i", res);
	}
}

void initCurveTest() {
	result = ma_engine_init(NULL, &engine);
	chkRes(result);

	ma_node_graph_config nodeGraphConfig = ma_node_graph_config_init(2);
	ma_node_graph nodeGraph;

	result = ma_node_graph_init(&nodeGraphConfig, NULL, &nodeGraph);    // Second parameter is a pointer to allocation callbacks.
	chkRes(result);

	// Load audio
	result = ma_sound_init_from_file(&engine, "./Resources/sound.wav", MA_SOUND_FLAG_DECODE, nullptr, nullptr, &sound);
	chkRes(result);

	ma_data_source_node_config config = ma_data_source_node_config_init(&sound);

	ma_data_source_node dataSourceNode;
	result = ma_data_source_node_init(&nodeGraph, &config, NULL, &dataSourceNode);
	chkRes(result);

	ma_lpf_node_config lpfConfig = ma_lpf_node_config_init(2, ma_engine_get_sample_rate(&engine), 200, 1);

	ma_lpf_node lpfNode;
	result = ma_lpf_node_init(&nodeGraph, &lpfConfig, nullptr, &lpfNode);
	chkRes(result);

	result = ma_node_attach_output_bus(&dataSourceNode, 0, &lpfNode, 0);
	chkRes(result);
	result = ma_node_attach_output_bus(&lpfNode, 0, ma_node_graph_get_endpoint(&nodeGraph), 0);


	// result = ma_node_attach_output_bus(&dataSourceNode, 0, ma_node_graph_get_endpoint(&nodeGraph), 0);
	// chkRes(result);



	// ma_data_source_node_config config = ma_data_source_node_config_init(pMyDataSource);

	// ma_data_source_node dataSourceNode;
	// result = ma_data_source_node_init(&nodeGraph, &config, NULL, &dataSourceNode);
	if (result != MA_SUCCESS) {
		// Failed to create data source node.
	}

	// ma_lpf_node_config lpfConfig = ma_lpf_node_config_init(2, 44800, double cutoffFrequency, ma_uint32 order)


}

static std::string axewftoload = "";
static bool dataLoaded = false;
json x;

void updateCurveTest() {
	using namespace ImGui;

	Begin("Curve Edit");

	// Terrible code btw

	if (Button("sound")) {
		// ma_engine_play_sound(&engine, "./Resources/sound.wav", nullptr);

		ma_sound_seek_to_pcm_frame(&sound, 0);
		ma_sound_start(&sound);
	}

	InputText("AXEwf to load", &axewftoload);
	SameLine();
	if (Button("Load")) {
		x = JSON::readBsonFile(axewftoload);
		dataLoaded = true;
	}

	if (dataLoaded) {

		Text("Hash              %s", std::string(x["audioHash"]).c_str());
		Text("Channels          %s", std::to_string((int)x["channels"]).c_str());
		Text("Bits              %s", std::to_string((int)x["bits"]).c_str());
		Text("Sample Rate       %s", std::to_string((int)x["sample_rate"]).c_str());
		Text("Samples per pixel %s", std::to_string((int)x["samples_per_pixel"]).c_str());

		for (auto& point : x["data"]) {
			TextUnformatted(std::to_string((int)point).c_str());
		}


	}










	// DragScalar("const char *label", ImGuiDataType_U32, "void *p_data");


	

	/* ImRect legendClippingRect = ImRect(GetWindowPos() + ImVec2(20,50), GetWindowPos() + GetWindowSize() - ImVec2(20,50));
	ImRect legendRect = legendClippingRect;
	ImRect rc = legendClippingRect;
	ImDrawList* draw_list = GetWindowDrawList();

	static const char* labels[] = { "Translation", "Rotation" , "Scale" };

	rampEdit.mMax = ImVec2(float(500.0f), 1.f);
	rampEdit.mMin = ImVec2(float(1.0f), 0.f);
	draw_list->PushClipRect(legendClippingRect.Min, legendClippingRect.Max, true);
	for (int i = 0; i < 3; i++) {
		ImVec2 pta(legendRect.Min.x + 30, legendRect.Min.y + i * 14.f);
		ImVec2 ptb(legendRect.Max.x, legendRect.Min.y + (i + 1) * 14.f);
		draw_list->AddText(pta, rampEdit.mbVisible[i] ? 0xFFFFFFFF : 0x80FFFFFF, labels[i]);
		if (ImRect(pta, ptb).Contains(ImGui::GetMousePos()) && ImGui::IsMouseClicked(0))
		rampEdit.mbVisible[i] = !rampEdit.mbVisible[i];
	}
	draw_list->PopClipRect();

	ImGui::SetCursorScreenPos(rc.Min);
	ImCurveEdit::Edit(rampEdit, rc.Max - rc.Min, 137, &legendClippingRect);

	// ImCurveEdit::Edit(rampEdit, ImVec2(0,0), GetID(111)); */

	// points.push_back(ImVec2(100, 100));
	// points.push_back(ImVec2(200, 200));
	// points.push_back(ImVec2(300, 300));
	// points.push_back(ImVec2(400, 400));

	// drawAutomation(ImRect(GetWindowPos() + ImVec2(20,50), GetWindowPos() + GetWindowSize() - ImVec2(20,50)));



	End();
}

}