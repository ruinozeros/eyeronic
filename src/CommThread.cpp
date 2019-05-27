/*
 * CommThread.cpp
 *
 *  Created on: May 27, 2019
 *      Author: ru1
 */

#include "CommThread.h"
#include "Log.h"

CommThread::CommThread() :
		Thread() {

}

CommThread::~CommThread() {

}

void CommThread::main() {
	log(INFO, "CommThread started...");
}
