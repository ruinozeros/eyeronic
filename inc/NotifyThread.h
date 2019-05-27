/*
 * NotificationThread.h
 *
 *  Created on: May 27, 2019
 *      Author: ru1
 */

#ifndef INC_NOTIFYTHREAD_H_
#define INC_NOTIFYTHREAD_H_

#include "Thread.h"

class NotifyThread : public Thread
{

public:
	NotifyThread();

	virtual ~NotifyThread();

protected:
	void main();

private:
	unsigned long busy_time_s_;

	void sendNotification(const char* msg);

};



#endif /* INC_NOTIFYTHREAD_H_ */
