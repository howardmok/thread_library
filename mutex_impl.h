#ifndef MUTEX_IMPL_H
#define MUTEX_IMPL_H

#include "mutex.h"
#include "cpu.h"
#include "cpu_impl.h"
#include <deque>
#include <iostream>
#include <stdexcept>
#include <ucontext.h>

class mutex::impl {
public:
	impl();
	~impl();
	void lockForMe();
	void unlockForMe();
private:
	bool status;							//status of lock
	ucontext_t * lockHolder;
	std::deque<ucontext_t *> mutexQ; 		//waiting queue for lock
};

#endif