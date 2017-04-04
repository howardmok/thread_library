#ifndef THREAD_IMPL_H
#define THREAD_IMPL_H

#include "cpu_impl.h"
#include "thread.h"
#include <deque>
#include <ucontext.h>

class thread::impl {
public:
	impl();
	~impl();
	void pushToJoinQueue(ucontext_t *);
	void mergeJoinToReady();
	void setThreadOutOfScope();
	bool * getThreadOutOfScope();
	void setThreadExecutionDone();
	bool getThreadExecutionDone();
private:
	bool * threadOutOfScope;
	bool threadExecutionDone;
	std::deque<ucontext_t *> joinedThreads;
};

#endif