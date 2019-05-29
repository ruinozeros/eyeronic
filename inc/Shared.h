/*
 * Shared.h
 *
 */

#ifndef INC_SHARED_H_
#define INC_SHARED_H_

#include <atomic>
#include <condition_variable>

typedef struct {

	std::atomic_bool notification_enabled;

	std::atomic_bool kill_me;

	std::condition_variable condition;

	std::atomic_int remaining_percentage;

	std::atomic_int notifier_state;

	std::atomic_int busy_min;

	std::atomic_int break_min;

} Shared;

#endif /* INC_SHARED_H_ */
