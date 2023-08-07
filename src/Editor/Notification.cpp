#include "Editor/Notification.hpp"
#include "Debug/Logger.h"
#include "imgui.h"
#include "imgui/imgui_utils.h"
#include <string>

namespace Shadow::Editor {

std::vector<Notification> storedNotifications;
std::vector<LivingNotification> activeNotifications;

static int increment = 0;

void showNotification(Notification notif) {
	LivingNotification livingNotif;
	livingNotif.notif = notif;
	activeNotifications.push_back(livingNotif);
	increment++;
}

static Notification dummyNotif;
static std::string dummyNotif_brief = "";
static std::string dummyNotif_details = "";

static void callback() { PRINT("Called back?"); }

void notificationUpdate() {

	ImGui::Begin("Notification Debugger");
	ImGui::InputText("brief", &dummyNotif_brief);
	ImGui::InputText("details", &dummyNotif_details);
	if (ImGui::Button("Submit Notif")) {
		dummyNotif.brief = dummyNotif_brief;
		dummyNotif.detailed = dummyNotif_details;
		NotificationAction na;
		na.buttonContent = "Click me";
		na.callback = callback;
		dummyNotif.actions.push_back(na);
		showNotification(dummyNotif);
	}
	ImGui::End();

	if (activeNotifications.empty())
		return;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	ImVec2 workPos = viewport->WorkPos;
	ImVec2 workSize = viewport->WorkSize;

	ImGui::SetNextWindowPos(ImVec2(workPos.x + workSize.x - 30, workPos.y + workSize.y - 30),
		ImGuiCond_Always, ImVec2(1, 1));
	ImGui::Begin("ActiveNotifications", nullptr, flags);

	ImGui::InvisibleButton("invis btn to make notif wide", ImVec2(400, 10));

	for (int i = 0; i < activeNotifications.size(); i++) {
		auto activeNotif = activeNotifications[i];

		ImGui::Separator();
		ImGui::Text("%s", activeNotif.notif.brief.c_str());
		ImGui::Text("%s", activeNotif.notif.detailed.c_str());

		for (auto& action : activeNotif.notif.actions) {
			if (ImGui::Button(action.buttonContent.c_str()))
				action.callback();
		}
		ImGui::Separator();
	}
	ImGui::End();
}

}