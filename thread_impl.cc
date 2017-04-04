#include "thread_impl.h"
#include <iostream>

using namespace std;

thread::impl::impl() : threadExecutionDone(false) {
	assert_interrupts_disabled();
	assert(guard);
	thread::impl::threadOutOfScope = new bool;
	*thread::impl::threadOutOfScope = false;
}

thread::impl::~impl() {

}

void thread::impl::pushToJoinQueue(ucontext_t * t) {
	assert(t);
	assert_interrupts_disabled();
	assert(guard);
	thread::impl::joinedThreads.push_back(t);
}

void thread::impl::mergeJoinToReady() {
	assert_interrupts_disabled();
	assert(guard);
	while( !thread::impl::joinedThreads.empty() ) {
		ucontext_t * temp = thread::impl::joinedThreads.front();
		assert(temp);
		thread::impl::joinedThreads.pop_front();
		cpu::self()->impl_ptr->push_back(temp);
		cpu::self()->impl_ptr->signalCPU();
	}
}

void thread::impl::setThreadOutOfScope() {
	*thread::impl::threadOutOfScope = true;
}

bool * thread::impl::getThreadOutOfScope() {
	return thread::impl::threadOutOfScope;
}

void thread::impl::setThreadExecutionDone() {
	assert_interrupts_disabled();
	assert(guard);
	thread::impl::threadExecutionDone = true;
}

bool thread::impl::getThreadExecutionDone() {
	assert_interrupts_disabled();
	assert(guard);
	return thread::impl::threadExecutionDone;            
}
