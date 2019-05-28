/*
 * Defines.h
 *
 */

#ifndef INC_DEFINES_H_
#define INC_DEFINES_H_

#define SYSLOG_NAME "eyeronicd"

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"

#define BUFFER_SIZE 12

enum LogLevel {
	INFO, ERROR
};

#define TITLE       "Reminder"
#define MESSAGE     "Take a break and rest your eyes."
#define ICON        "~/.config/eyeronic/coffee.png"

static const unsigned long second = 1;
static const unsigned long minute = 60 * second;

// settings
static const unsigned long break_duration_s = 20 * minute; // duration of a break     = 20 minutes
static const unsigned long busy_duration_s = 20 * minute;  // duration of a work unit = 20 minutes
static const unsigned long threshold_away_s = minute;      // 1 minute idle does not add up busy time

#endif /* INC_DEFINES_H_ */
