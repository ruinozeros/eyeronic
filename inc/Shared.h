/*
 * Shared.h
 *
 *  Created on: May 27, 2019
 *      Author: ru1
 */

#ifndef INC_SHARED_H_
#define INC_SHARED_H_

#include <atomic>

typedef struct {

	std::atomic_bool notificationEnabled;
	std::atomic_bool killMe;

} Shared;

#endif /* INC_SHARED_H_ */
