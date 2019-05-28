/*
 * CommThread.cpp
 *
 *  Created on: May 27, 2019
 *      Author: ru1
 */

#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>
#include <stdlib.h>
#include <string>
#include <mutex>              // std::mutex, std::unique_lock
#include <gio/gio.h>

#include "NotifyThread.h"
#include "Log.h"

NotifyThread::NotifyThread() :
		Thread(), busy_time_s_(0), config_(nullptr) {

}

NotifyThread::NotifyThread(Shared* config) :
		Thread(), busy_time_s_(0), config_(config) {

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

	while (!config_->killMe) {

		// wait for threshold or being woken up
		if (config_->condition.wait_for(lck,
				std::chrono::seconds(threshold_away_s))
				== std::cv_status::no_timeout)
			continue;


		if (!config_->notificationEnabled) {
			config_->condition.wait(lck);
		}

		XScreenSaverQueryInfo(dpy, DefaultRootWindow(dpy), info);
		unsigned long away_time_s = info->idle / 1000;

		if (away_time_s > break_duration_s) {
			// already taking a break
			busy_time_s_ = 0;
			continue;
		}

		if (away_time_s < threshold_away_s) {
			// increase busy time
			busy_time_s_ += threshold_away_s;
		}

		if (busy_time_s_ > busy_duration_s) {
			sendNotification(TITLE, MESSAGE, ICON);
			busy_time_s_ = 0;
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
