/*
 * Log.h
 *
 *  Created on: May 27, 2019
 *      Author: ru1
 */

#ifndef INC_LOG_H_
#define INC_LOG_H_

#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>

#include <Defines.h>

void initLog() {
	openlog(SYSLOG_NAME, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	syslog(LOG_NOTICE, "Program started by User %s (UID=%d)", getenv("USER"),
			getuid());
}

void log(LogLevel level, const char* message) {
	switch (level) {
	case INFO:
		syslog(LOG_INFO, message);
		break;
	case ERROR:
		syslog(LOG_ERR, message);
		break;
	}
}

#endif /* INC_LOG_H_ */
