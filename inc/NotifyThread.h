/*
 * NotificationThread.h
 *
 *  Created on: May 27, 2019
 *      Author: ru1
 */

#ifndef INC_NOTIFYTHREAD_H_
#define INC_NOTIFYTHREAD_H_

#include "Thread.h"
#include "Shared.h"

class NotifyThread : public Thread
{

public:
	NotifyThread();

	NotifyThread(Shared* config);

	virtual ~NotifyThread();

protected:
	void main();

private:
	unsigned long busy_time_s_;

	unsigned long away_time_s_;

	Shared* config_;

	void sendNotification(const char* title, const char* msg, const char* icon);

};



#endif /* INC_NOTIFYTHREAD_H_ */
