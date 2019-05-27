/*
 * CommThread.cpp
 *
 *  Created on: May 27, 2019
 *      Author: ru1
 */

#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>

#include "NotifyThread.h"
#include "Log.h"

NotifyThread::NotifyThread() :
		Thread(), busy_time_s_(0) {

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
	while (1) {
		sleep (threshold_away_s);

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
			sendNotification("It's time for a break!");
			busy_time_s_ = 0;
		}

	}
}

void NotifyThread::sendNotification(const char* msg) {

}
