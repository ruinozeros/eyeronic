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

#include <fstream>
#include <sstream>

#include <sys/socket.h>
#include <sys/un.h>

#include "Defines.h"
#include "Log.h"
#include "CommThread.h"
#include "NotifyThread.h"
#include "UserConfig.h"
#include "StringOp.h"

void deamonize();

void signalHandler(int sig);

void printUsage(char* argv0);

void startDeamon();

void sendCommandToDeamon(const char* cmd);

void setConfigPath(const char* path);

void readConfigFile();

bool fileExists(const std::string& p);

int main(int argc, char* argv[]) {

	if (argc < 2) {
		printUsage(argv[0]);
		exit(EXIT_FAILURE);
	}

	int c;

	while (1) {
		int option_index = 0;

		static struct option long_options[] ={
				{ "start", no_argument, 0, 0 },
				{ "stop", no_argument, 0, 0 },
				{ "enable", no_argument, 0, 0 },
				{ "disable",no_argument, 0, 0 },
				{ "toggle", no_argument, 0,0 },
				{ "status", no_argument, 0, 0 },
				{ "config", required_argument, 0, 0 },
				{ "percent",no_argument, 0, 0},
				{ "help",no_argument, 0, 'h' },
				{ 0, 0, 0, 0 } };

		c = getopt_long(argc, argv, "h", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0: /* long_options... */
			switch (option_index) {
			case 0: /* --start */
				startDeamon();
				break;

			case 1: /* --stop */
				sendCommandToDeamon(CMD_STOP);
				break;

			case 2: /* --enable */
				sendCommandToDeamon(CMD_ENABLE);
				break;

			case 3: /* --disable */
				sendCommandToDeamon(CMD_DISABLE);
				break;

			case 4: /* --toggle */
				sendCommandToDeamon(CMD_TOGGLE);
				break;

			case 5: /* --status */
				sendCommandToDeamon(CMD_STATUS);
				break;

			case 6: /* --config=*/
				setConfigPath(optarg);
				break;

			case 7: /* --percent*/
				sendCommandToDeamon(CMD_PERCENT);
			}
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

	if (optind < argc) {
		printUsage(argv[0]);
		exit(EXIT_FAILURE);
	}

	return 0;
}

void startDeamon() {
	// Read user config file.
	readConfigFile();

	printf("Start deamon!");

	// Deamonize this process.
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

	// init shared variables
	Shared shared_vars;
	shared_vars.notification_enabled = true;
	shared_vars.kill_me = false;
	shared_vars.break_min = 0;
	shared_vars.busy_min = 0;
	shared_vars.notifier_state = BUSY;
	shared_vars.remaining_percentage = 100;

	CommThread communication_thread(&shared_vars);
	NotifyThread notification_thread(&shared_vars);

	communication_thread.run();
	notification_thread.run();

	communication_thread.join();
	notification_thread.join();

	log(INFO, "Deamon terminates.");
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

void printUsage(char* argv0) {
	printf("Usage: %s [COMMAND]\n", argv0);
	printf("Start and control eyeronic deamon.\n\n");

	printf("Commands:\n");
	printf("  --start            Start deamon\n");
	printf("  --stop             Stop deamon\n");
	printf("  --enable           Enable notification\n");
	printf("  --disable          Disable notification\n");
	printf("  --toggle           Toggle notification status\n");
	printf("  --status           Get notification status\n");
	printf("  --config           Set path to config file.\n"
		   "                     Default ~/.config/eyeronic/config\n");
	printf("  --help, -h         Show this help message\n\n");

	printf("28.05.2019 https://github.com/ruinozeros");
}

void setConfigPath(const char* path) {
	std::string p(path);

	if (p.length() == 0) {
		perror("No path specified");
		exit(EXIT_FAILURE);
	}

	if (!fileExists(p)) {
		perror("File not found!");
		exit(EXIT_FAILURE);
	}

	UserConfig::get().path(p);
}

std::string relToAbs(std::string rel_path) {
	std::string user_dir = std::string(getenv("HOME"));
	std::string abs_path = sops::replace(std::string(rel_path), "$HOME",
			user_dir);

	user_dir.append(sops::separator());
	abs_path = sops::replace(abs_path, "~/", user_dir);
	return abs_path;
}

void readConfigFile() {

	std::string abs_path = relToAbs(UserConfig::get().path());
	if(!fileExists(abs_path))
	{
		perror((std::string("Config file ") + abs_path +
				"not found! Using default values.").c_str());
		return;
	}

	printf("Read config file '%s'.\n", abs_path.c_str());

	std::ifstream fin(relToAbs(UserConfig::get().path()));
	std::string line;
	while (getline(fin, line)) {

		if (line.find("=") == std::string::npos)
			continue;

		std::string stmp = sops::lstrip(line.substr(line.find("=") + 1));
		unsigned long ltmp;

		// title ===============================================================
		if (line.find("title") != std::string::npos) {
			if (stmp.size() + 1 < BUFFER_SIZE) {

				UserConfig::get().title(stmp);
				printf("Set title to '%s'.\n", stmp.c_str());
			}
			else
			{
				perror((std::string("Max title length of ")
								+ std::to_string(BUFFER_SIZE) +
								" reached").c_str());
				exit(EXIT_FAILURE);
			}

		// icon ================================================================
		} else if (line.find("icon") != std::string::npos) {
			if (stmp.size() + 1 < BUFFER_SIZE) {

				stmp = relToAbs(stmp);
				if (fileExists(stmp)) {
					UserConfig::get().icon(stmp);
					printf("Set icon path to '%s'.\n", stmp.c_str());
				} else {
					perror("Icon path not found!");
					exit(EXIT_FAILURE);
				}
			} else {
				perror((std::string("Icon path length of ")
						+ std::to_string(BUFFER_SIZE) + " reached").c_str());
				exit(EXIT_FAILURE);
			}


		// message =============================================================
		} else if (line.find("message") != std::string::npos) {
			if (stmp.size() + 1 < BUFFER_SIZE) {

			UserConfig::get().message(stmp);
			printf("Set message to '%s'.\n", stmp.c_str());
			} else {
				perror((std::string("Message length of ")
						+ std::to_string(BUFFER_SIZE) + " reached").c_str());
				exit(EXIT_FAILURE);
			}

		// break duration ======================================================
		} else if (line.find("break_duration") != std::string::npos) {
			std::istringstream iss(stmp);
			iss >> ltmp;

			if (ltmp < MAX_TIME_S) {
				UserConfig::get().breakDuration(ltmp);
				printf("Set break duration to %ld.\n", ltmp);
			} else {
				perror((std::string("Message length of ")
						+ std::to_string(MAX_TIME_S) + " reached").c_str());
				exit(EXIT_FAILURE);
			}

		// remind after ========================================================
		} else if (line.find("remind_after") != std::string::npos) {
			std::istringstream iss(stmp);
			iss >> ltmp;

			if (ltmp < MAX_TIME_S) {
				UserConfig::get().remindAfter(ltmp);
				printf("Set remind_after to %ld.\n", ltmp);

			} else {
				perror((std::string("Message length of ")
						+ std::to_string(MAX_TIME_S) + " reached").c_str());
				exit(EXIT_FAILURE);
			}

		// freeze after ========================================================
		} else if (line.find("freeze_after") != std::string::npos) {
			std::istringstream iss(stmp);
			iss >> ltmp;

			if (ltmp < MAX_TIME_S) {
			UserConfig::get().minBreakDuration(ltmp);
			printf("Set min break duration to %ld.\n", ltmp);

			} else {
				perror((std::string("Message length of ")
						+ std::to_string(MAX_TIME_S) + " reached").c_str());
				exit(EXIT_FAILURE);
			}
		}
	}
}

void sendCommandToDeamon(const char* cmd) {
	struct sockaddr_un addr;

	int ret;
	int data_socket;
	char buffer[BUFFER_SIZE];

	/* Create local socket. */
	data_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (data_socket == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	/*
	 * For portability clear the whole structure, since some
	 * implementations have additional (nonstandard) fields in
	 * the structure.
	 */
	memset(&addr, 0, sizeof(struct sockaddr_un));

	/* Connect socket to socket address */
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

	ret = connect(data_socket, (const struct sockaddr *) &addr,
			sizeof(struct sockaddr_un));
	if (ret == -1) {
		fprintf(stderr, "The server is down.\n");
		exit(EXIT_FAILURE);
	}

	/* Send argument */
	ret = write(data_socket, cmd, strlen(cmd) + 1);
	if (ret == -1) {
		perror("write");
	}

	/* Receive result. */
	ret = read(data_socket, buffer, BUFFER_SIZE);
	if (ret == -1) {
		perror("read");
		exit(EXIT_FAILURE);
	}

	/* Ensure buffer is 0-terminated. */
	buffer[BUFFER_SIZE - 1] = 0;

	printf("%s\n", buffer);

	/* Close socket. */
	close(data_socket);
}

bool fileExists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

