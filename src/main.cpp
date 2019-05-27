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
#include <string.h>
#include <signal.h>
#include <iostream>

#include "Defines.h"
#include "Log.h"
#include "CommThread.h"
#include "NotifyThread.h"

void deamonize();

void signalHandler(int sig);

int main() {

	// deamonize this process
	deamonize();

	/* Change the current working directory */
	if ((chdir("/tmp/")) < 0) {
		log(ERROR, "chdir() failed");
		exit(EXIT_FAILURE);
	}

	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	/* Daemon-specific initialization goes here */
	signal(SIGCHLD, SIG_IGN); /* ignore child */
	signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU, SIG_IGN); /*  --- " --- */
	signal(SIGTTIN, SIG_IGN); /*  --- " --- */
	signal(SIGHUP, signalHandler); /* catch hangup signal */
	signal(SIGTERM, signalHandler); /* catch kill signal */

	CommThread communication_thread;
	NotifyThread notification_thread;

	communication_thread.run();
	notification_thread.run();

	communication_thread.join();
	notification_thread.join();

	log(INFO, "Program terminates.");
	closelog();
	exit(EXIT_SUCCESS);
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

void signalHandler(int sig) {
	switch (sig) {
	case SIGHUP:
		log(INFO, "hangup signal detected");
		break;
	case SIGTERM:
		log(INFO, "terminate signal detected");
		closelog();
		exit(0);
		break;
	}
}
