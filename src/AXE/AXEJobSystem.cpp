#include "AXEJobSystem.hpp"
#include "Debug/EditorConsole.hpp"
#include "Debug/Logger.hpp"
#include "ShadowIcons.hpp"
#include "ImGuiNotify.hpp"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include <functional>
#include <string>
#include <thread>

namespace Shadow::AXE::JobSystem {

static int jobId = 0;
int getNextJobId() { return jobId++; }

static std::vector<Job> activeJobs;
static std::vector<Job> retiredJobs;

// this flips to true when a job has failed, forever tainting the runtime and
// indicating such to the user.
static bool jobIntegrityTainted = false;

void callback(int id, bool success, const std::string& miscInfo = "No additional info provided") {
	// Job has finished and wants to be marked as retired

	if (!success) {
		ImGui::InsertNotification({
			ImGuiToastType::Error,
			1000000,
			"A job has failed! This is usually not good!\nCheck the job status at the top to see\nwhat went wrong and TELL VINCE!"
		});
		jobIntegrityTainted = true;
	}

	int it = 0;
	for (auto& job : activeJobs) {
		it++;

		if (job.id != id) continue;

		job.success = success;
		job.miscInfo = miscInfo;
		job.endTime = std::time(nullptr);
		retiredJobs.emplace_back(job);
		activeJobs.erase(activeJobs.begin() + it - 1); // -1 since iterate before erase
		break;
	}
}

void submitJob(const std::string& name, std::function<bool()> jobMethod) {
	int id = getNextJobId();

	std::thread th([jobMethod, id]() {
		bool success = false;
		try {
			success = jobMethod();
			callback(id, success);
		} catch (const std::exception& e) {
			callback(id, false, e.what());
		} catch (...) {
			callback(id, false);
		}
	});
	th.detach();
	
	Job j;
	j.name = name;
	j.id = id;
	j.startTime = std::time(nullptr);

	activeJobs.emplace_back(j);
}

void onUpdateStatusBar() {
	using namespace ImGui;
	PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
	PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 100));

	if (jobIntegrityTainted) {
		PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		if (Button(SHADOW_ICON_CLOSE " Job Failure!")) OpenPopup("JobsPopup");
		PopStyleColor();
	} else {
		if (activeJobs.empty()) {
			if (Button(SHADOW_ICON_INFO " No active jobs")) OpenPopup("JobsPopup");
		} else {
			Spinner("##spinny", 12.0f, 4, IM_COL32(255, 0, 0, 255));
			SameLine();
			if (Button("AXE is doing background work")) OpenPopup("JobsPopup");
		}
	}

	SetItemTooltip("Click for more info on jobs");

	PopStyleColor(2);

	PushStyleColor(ImGuiCol_PopupBg, IM_COL32(0, 0, 0, 255));
	PushStyleColor(ImGuiCol_Border, IM_COL32(255, 255, 255, 255));
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));

	if (BeginPopup("JobsPopup")) {

		TextCenter("Jobs");

		Separator();

		if (activeJobs.empty())
			Text("There are no active jobs");

		for (auto& job : activeJobs) {
			Spinner(job.name.c_str(), 12.0f, 4, IM_COL32(255, 0, 0, 255));
			SameLine();
			TextUnformatted(job.name.c_str());
		}

		Separator();

		if (TreeNode("Retired Jobs")) {
			for (auto& job : retiredJobs) {
				if (job.success) {
					PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
					TextUnformatted(SHADOW_ICON_CHECK); SameLine();
				} else {
					PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
					TextUnformatted(SHADOW_ICON_CLOSE); SameLine();
				}

				TextUnformatted(job.name.c_str());

				if (BeginItemTooltip()) {
					PushTextWrapPos(GetFontSize() * 35.0f);

					char startTimeStr[100];
					std::strftime(startTimeStr, sizeof(startTimeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&job.startTime));
					Text("Job started at %s", startTimeStr);

					char endTimeStr[100];
					std::strftime(endTimeStr, sizeof(endTimeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&job.endTime));
					Text("Job ended at   %s", endTimeStr);

					Separator();
					TextUnformatted(job.miscInfo.c_str());
					PopTextWrapPos();
					EndTooltip();
				}

				PopStyleColor();
			}
			TreePop();
		}

		EndPopup();
	}

	PopStyleColor(2);
	PopStyleVar();
}

void degradeEditorWithMessage(const std::string& name, const std::string& message) {
	submitJob(name, [message]() -> bool {
		throw std::runtime_error(message);
		return false;
	});
}

}