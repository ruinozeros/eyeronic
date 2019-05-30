/*
 * CommThread.cpp
 *
 */

#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>
#include <stdlib.h>
#include <string>
#include <mutex>              // std::mutex, std::unique_lock
#include <gio/gio.h>
#include <UserConfig.h>

#include "NotifyThread.h"
#include "Log.h"
#include "UserConfig.h"

NotifyThread::NotifyThread() :
		Thread(), busy_time_s_(0), away_time_s_(0), config_(nullptr) {

}

NotifyThread::NotifyThread(Shared* config) :
		Thread(), busy_time_s_(0), away_time_s_(0), config_(config) {

}

NotifyThread::~NotifyThread() {

}

void NotifyThread::main() {
	log(INFO, "NotifyThread started...");

	Display *dpy = XOpenDisplay(NULL);

	if (dpy == nullptr) {
		log(ERROR, "XOpenDisplay() returned nullptr");
		exit(EXIT_FAILURE);
	}

	XScreenSaverInfo *info = XScreenSaverAllocInfo();

	busy_time_s_ = 0;

	std::mutex mtx;
	std::unique_lock<std::mutex> lck(mtx);

	while (!config_->kill_me) {

		config_->remaining_percentage =
				(int) ((1.0
				- (double) busy_time_s_
						/ (double) UserConfig::get().remindAfter()) * 100.0);

		// notification disabled -> wait
		if (!config_->notification_enabled) {

			busy_time_s_ = 0;
			config_->busy_min = 0;

			config_->condition.wait(lck);
			// enabled
		}

		// wait for threshold or being woken up
		if (config_->condition.wait_for(lck,
				std::chrono::seconds(UserConfig::get().checkInterval()))
				== std::cv_status::no_timeout)
			continue;

		XScreenSaverQueryInfo(dpy, DefaultRootWindow(dpy), info);
		away_time_s_ = info->idle / 1000;

		if (away_time_s_ > UserConfig::get().breakDuration()) {
			// already taking a break
			busy_time_s_ = 0;
			config_->busy_min = 0;
			continue;
		}

		if (away_time_s_ < UserConfig::get().minBreakDuration()) {
			// busy
			config_->notifier_state = BUSY;
			busy_time_s_ += UserConfig::get().checkInterval();
			config_->busy_min = busy_time_s_ / minute;
		}
		else
		{
			// pausing
			config_->notifier_state = PAUSE;
			config_->break_min = away_time_s_ / minute;
		}

		if (busy_time_s_ > UserConfig::get().remindAfter() &&
				config_->notification_enabled) {
			// send notification
			sendNotification(UserConfig::get().title(),
							 UserConfig::get().message(),
							 UserConfig::get().icon());

			busy_time_s_ = 0;
			config_->break_min = 0;
		}
	}
	log(INFO, "NotifyThread stopped.");
}

void NotifyThread::sendNotification(const char* title, const char* text,
		const char* path_icon) {
	// see https://wiki.archlinux.org/index.php/Desktop_Notifications#C++
	GApplication *application = g_application_new("hello.world",
			G_APPLICATION_FLAGS_NONE);
	g_application_register(application, NULL, NULL);
	GNotification *notification = g_notification_new(title);
	g_notification_set_body(notification, text);
	GIcon *icon = g_themed_icon_new(path_icon);
	g_notification_set_icon(notification, icon);
	g_application_send_notification(application, NULL, notification);
	g_object_unref(icon);
	g_object_unref(notification);
	g_object_unref(application);

	log(INFO, "notification sent!");
}
