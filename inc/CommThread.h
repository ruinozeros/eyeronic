/*
 * CommThread.h
 *
 *  Created on: May 27, 2019
 *      Author: ru1
 */

#ifndef INC_COMMTHREAD_H_
#define INC_COMMTHREAD_H_

#include "Thread.h"

class CommThread : public Thread
{
public:
	CommThread();

	virtual ~CommThread();

protected:
	void main();

private:
	bool handleCommand(const char* cmd);

};



#endif /* INC_COMMTHREAD_H_ */
