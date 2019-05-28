/*
 * CommThread.cpp
 *
 *  Created on: May 27, 2019
 *      Author: ru1
 */

#include <sys/socket.h>
#include <sys/un.h>
#include <mutex>

#include "CommThread.h"
#include "Log.h"

CommThread::CommThread() :
		Thread(), config_(nullptr) {

}

CommThread::CommThread(Shared* config) :
		Thread(), config_(config) {

}
CommThread::~CommThread() {

}

void CommThread::main() {
	log(INFO, "CommThread started...");

	struct sockaddr_un name;
	int ret;
	int connection_socket;
	int data_socket;
	char buffer[BUFFER_SIZE];
	char answer[BUFFER_SIZE];

	/*
	 * In case the program exited inadvertently on the last run,
	 * remove the socket.
	 */
	unlink(SOCKET_NAME);

	connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (connection_socket == -1) {
		log(ERROR, "socket() failed");
		exit(EXIT_FAILURE);
	}
	memset(&name, 0, sizeof(struct sockaddr_un));

	/* Bind socket to socket name. */
	name.sun_family = AF_UNIX;
	strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

	ret = bind(connection_socket, (const struct sockaddr *) &name,
			sizeof(struct sockaddr_un));
	if (ret == -1) {
		log(ERROR, "bind failed()");
		exit(EXIT_FAILURE);
	}

	/* Prepare for accepting connections. The backlog size is set to 1. */
	ret = listen(connection_socket, 1);
	if (ret == -1) {
		log(ERROR, "listen() failed");
		exit(EXIT_FAILURE);
	}

	/* This is the main loop for handling connections. */
	while (!config_->killMe) {
		data_socket = accept(connection_socket, NULL, NULL);
		if (data_socket == -1) {
			log(ERROR, "accept() failed");
			exit(EXIT_FAILURE);
		}

		while (1) {
			/* Wait for next data packet. */
			ret = read(data_socket, buffer, BUFFER_SIZE);
			if (ret == -1) {
				log(ERROR, "read() failed");
				exit(EXIT_FAILURE);
			}

			/* Ensure buffer is 0-terminated. */
			buffer[BUFFER_SIZE - 1] = 0;

			if (handleCommand(buffer, answer))
				break;
		}

		/* Send answer. */
		sprintf(buffer, answer);
		ret = write(data_socket, buffer, BUFFER_SIZE);
		if (ret == -1) {
			log(ERROR, "write() failed");
			exit(EXIT_FAILURE);
		}

		/* Close socket. */
		close(data_socket);
	}

	close(connection_socket);

	/* Unlink the socket. */
	unlink(SOCKET_NAME);

	log(INFO, "CommThread stopped.");
}

bool CommThread::handleCommand(const char* cmd, char* answer) {

	/* disable =============================================================  */
	if (!strncmp(cmd, CMD_DISABLE, BUFFER_SIZE)) {
		log(INFO, "Disable notification.");
		config_->notificationEnabled = false;
		sprintf(answer, ANS_OK);
	}
	/* enable ==============================================================  */
	else if (!strncmp(cmd, CMD_ENABLE, BUFFER_SIZE)) {
		log(INFO, "Enable notification.");
		config_->notificationEnabled = true;
		sprintf(answer, ANS_OK);
	}
	/* stoggle =============================================================  */
	else if (!strncmp(cmd, CMD_TOGGLE, BUFFER_SIZE)) {
		bool enabled = config_->notificationEnabled;

		if(enabled)
		{
			log(INFO, "Toggle notification to DISABLED.");
			config_->notificationEnabled = false;
			sprintf(answer, ANS_OFF);
		}
		else
		{
			log(INFO, "Toggle notification to ENABLED.");
			config_->notificationEnabled = true;
			sprintf(answer, ANS_ON);
		}
	}
	/* status ==============================================================  */
	else if(!strncmp(cmd, CMD_STATUS, BUFFER_SIZE)) {
		if(config_->notificationEnabled)
			sprintf(answer, ANS_ON);
		else
			sprintf(answer, ANS_OFF);
	}
	/* stop ================================================================  */
	else if(!strncmp(cmd, CMD_STOP, BUFFER_SIZE)) {
		log(INFO, "Stop deamon.");
		config_->killMe = true;
	}
	/* ??????? =============================================================  */
	else
	{
		sprintf(answer, ANS_ERR);
	}

	return true;
}

