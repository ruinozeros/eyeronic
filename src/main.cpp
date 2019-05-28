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
#include <getopt.h>

#include "Defines.h"
#include "Log.h"
#include "CommThread.h"
#include "NotifyThread.h"

void deamonize();

void signalHandler(int sig);

void printUsage(char* argv0);

void start();

void stop();


int main(int argc, char* argv[]) {

	if (argc != 2) {
		printUsage(argv[0]);
		exit(EXIT_FAILURE);
	}

	int c;

	while (1) {
		int option_index = 0;

		static struct option long_options[] = {
				{"start",   no_argument, 0,  0 },
				{"stop",    no_argument, 0,  0 },
				{"enable",  no_argument, 0,  0 },
				{"disable", no_argument, 0,  0 },
				{"toggle",  no_argument, 0,  0 },
				{"status",  no_argument, 0,  0 },
				{0,         0,           0,  0 }
				};

		c = getopt_long(argc, argv, "h", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {

		case 0: /* start */
			printf("option %s", long_options[option_index].name);
			break;

		case 1: /* stop */
			printf("option %s", long_options[option_index].name);
			break;

		case 2: /* enable */
			printf("option %s", long_options[option_index].name);
			break;

		case 3: /* disable */
			printf("option %s", long_options[option_index].name);
			break;

		case 4: /* toggle */
			printf("option %s", long_options[option_index].name);
			break;

		case 5: /* status */
			printf("option %s", long_options[option_index].name);
			break;

		case 'h':
			printUsage(argv[0]);
			exit(EXIT_SUCCESS);

		case '?':
			printUsage(argv[0]);
			exit(EXIT_FAILURE);

		default:
			printUsage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}

void start() {

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

	Shared config;
	config.notificationEnabled = true;

	CommThread communication_thread(&config);
	NotifyThread notification_thread(&config);

	communication_thread.run();
	notification_thread.run();

	communication_thread.join();
	notification_thread.join();

	log(INFO, "Program terminates.");
	closelog();
	exit(EXIT_SUCCESS);
}

void stop() {


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

void printUsage(char* argv0) {
	printf("Usage: %s [COMMAND]\n", argv0);
	printf("Start and control eyeronic deamon.\n\n");

	printf("Commands:\n");
	printf("  start            Start deamon\n");
	printf("  stop             Stop deamon\n");
	printf("  enable           Enable notification\n");
	printf("  disable          Disable notification\n");
	printf("  toggle           Toggle notification status\n");
	printf("  status           Get notification status\n\n");

	printf("28.05.2019 https://github.com/ruinozeros");
}
