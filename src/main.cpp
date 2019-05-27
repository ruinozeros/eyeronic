/*
 * main.cpp
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <iostream>


#include <Defines.h>
#include <Log.h>

void deamonize();

void initLog();

void log(LogLevel loglevel, const char* msg);

int main() {

	// deamonize this process
	deamonize();

}

void deamonize() {
	/* Our process ID and Session ID */
	pid_t pid, sid;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		std::cerr << "eyeronicd::deamonice() fork() failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then
	 we can exit the parent process. */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* Change the file mode mask */
	umask(0);

	/* Open logs here */
	initLog();

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		log(ERROR, "setsid() failed");
		exit(EXIT_FAILURE);
	}
}
