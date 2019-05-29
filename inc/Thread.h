/*
 * Thread.h
 *
 */

#ifndef INC_THREAD_H_
#define INC_THREAD_H_

#include <thread>

class Thread {
public:
	Thread() :
			thread_() {
	}

	virtual ~Thread() {
	}

	void run() {
		thread_ = std::thread(&Thread::main, this);
	}

	void join() {
		thread_.join();
	}

protected:
	virtual void main() = 0;

private:
	Thread(Thread& l) {
	}

	std::thread thread_;
};

#endif /* INC_THREAD_H_ */
