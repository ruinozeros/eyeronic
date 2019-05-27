/*
 * CommThread.cpp
 *
 *  Created on: May 27, 2019
 *      Author: ru1
 */

#include "NotifyThread.h"
#include "Log.h"

NotifyThread::NotifyThread() :
		Thread() {

}

NotifyThread::~NotifyThread() {

}

void NotifyThread::main() {
	log(INFO, "NotifyThread started...");
}
