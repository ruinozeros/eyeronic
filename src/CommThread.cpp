/*
 * CommThread.cpp
 *
 */

#include <sys/socket.h>
#include <sys/un.h>
#include <mutex>
#include <sstream>

#include <iomanip>

#include "CommThread.h"
#include "Log.h"
#include "UserConfig.h"

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
	while (!config_->kill_me) {
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
		config_->notification_enabled = false;
		config_->condition.notify_all();
		sprintf(answer, ANS_OK);
	}
	/* enable ==============================================================  */
	else if (!strncmp(cmd, CMD_ENABLE, BUFFER_SIZE)) {
		log(INFO, "Enable notification.");
		config_->notification_enabled = true;
		config_->condition.notify_all();
		sprintf(answer, ANS_OK);
	}
	/* toggle =============================================================  */
	else if (!strncmp(cmd, CMD_TOGGLE, BUFFER_SIZE)) {
		bool enabled = config_->notification_enabled;

		if(enabled)
		{
			log(INFO, "Toggle notification to DISABLED.");
			config_->notification_enabled = false;
			config_->condition.notify_all();
			sprintf(answer, ANS_OFF);
		}
		else
		{
			log(INFO, "Toggle notification to ENABLED.");
			config_->notification_enabled = true;
			config_->condition.notify_all();
			sprintf(answer, ANS_ON);
		}
	}
	/* status ==============================================================  */
	else if(!strncmp(cmd, CMD_STATUS, BUFFER_SIZE)) {
		if (config_->notification_enabled && config_->notifier_state == BUSY) {
			// busy message (shows time until notification)

			int remain_min = (UserConfig::get().remindAfter() / minute)
					- config_->busy_min;

			std::stringstream ss;
			ss << std::setw(3) << std::setfill('0') << remain_min;
			std::string output = std::string(ANS_ON) + ":" + ss.str();

			sprintf(answer, output.c_str());


		} else if (config_->notification_enabled
				&& config_->notifier_state == PAUSE) {
			// break message (shows minute)

			std::stringstream ss;
			ss << std::setw(3) << std::setfill('0') << config_->break_min;
			std::string output = std::string(ANS_PAUSE) + ":" + ss.str();

			sprintf(answer, output.c_str());

		} else
			sprintf(answer, ANS_OFF);
	}
	/* stop ================================================================  */
	else if(!strncmp(cmd, CMD_STOP, BUFFER_SIZE)) {
		config_->kill_me = true;
		config_->condition.notify_all();
		sprintf(answer, ANS_OK);
	}
	/* percent =============================================================  */
	else if(!strncmp(cmd, CMD_PERCENT, BUFFER_SIZE)) {
		sprintf(answer, std::to_string(config_->remaining_percentage).c_str());
	}
	/* ??????? =============================================================  */
	else
	{
		sprintf(answer, ANS_ERR);
	}

	return true;
}

