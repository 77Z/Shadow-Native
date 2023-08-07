#ifndef SHADOW_NATIVE_EDITOR_NOTIFICATION_HPP
#define SHADOW_NATIVE_EDITOR_NOTIFICATION_HPP

#include <string>
#include <vector>

namespace Shadow::Editor {

enum NotificationUrgency_ {
	NotificationUrgency_Info,
	NotificationUrgency_Success,
	NotificationUrgency_Warning,
	NotificationUrgency_Error
};

struct NotificationAction {
	std::string buttonContent;
	void (*callback)();
};

struct Notification {
	std::string brief = "";
	std::string detailed = "";
	NotificationUrgency_ urgency = NotificationUrgency_Info;
	std::vector<NotificationAction> actions;
};

struct LivingNotification {
	int lifeSpan = 10000;
	Notification notif;
};

void showNotification(Notification notif);
void notificationUpdate();
}

#endif /* SHADOW_NATIVE_EDITOR_NOTIFICATION_HPP */