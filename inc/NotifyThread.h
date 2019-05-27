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

};



#endif /* INC_NOTIFYTHREAD_H_ */
