#include "thread.h"
#include "thread_impl.h"
#include "cpu_impl.h"
#include <ucontext.h>
#include <iostream>
#include <new>
#include <stdexcept>

static void enabler_function (thread::impl * i_ptr, bool* threadOutOfScope, thread_startfunc_t func, void * args){
	assert(guard);
	assert_interrupts_disabled();

	// when a newly created thread finally gets run, check the exit queue and clear the stack of useless threads
	cpu::self()->impl_ptr->exitQ_clear_all();

	guard.exchange(false);	// unlock the guard
	// enter user code with interrupts enabled
	cpu::self()->interrupt_enable();
	try {
		func(args);
		// thread execution terminates. send joinQueue to readyQueue,
		cpu::self()->interrupt_disable();
		while(guard.exchange(true)) {} // grab the guard
		// move all joined queues to ready queue
		if (*threadOutOfScope == false) {
			i_ptr->setThreadExecutionDone();
			i_ptr->mergeJoinToReady();
			cpu::self()->impl_ptr->exitQ_push_back(cpu::self()->impl_ptr->getRunningThread());
		}
		else {
			// threadOutOfScope is true
			assert(guard);
			delete threadOutOfScope;
		}
		assert(guard);
		assert_interrupts_disabled();
		if(cpu::self()->impl_ptr->empty()) { // readyQ is empty
			setcontext(cpu::self()->impl_ptr->getCPU());
		} else { // readyQ is not empty
			ucontext_t *next = cpu::self()->impl_ptr->front();
			cpu::self()->impl_ptr->pop_front();
			cpu::self()->impl_ptr->setRunningThread(next);
			setcontext(next);
		}
	} // end of try
	catch (std::runtime_error &e) {
		std::cerr << "Caught a runtime_error exception in thread execution\n";
		throw e;
	}
	catch (std::bad_alloc &e) {
		std::cerr << "Caught a bad_alloc exception in thread execution\n";
		throw e;
	}
	
}

thread::thread(thread_startfunc_t func, void * args) {
	// disabling interrupts so we can create thread
	assert_interrupts_enabled();
	cpu::self()->interrupt_disable();
	while(guard.exchange(true)) {} // grab the guard
	
	try {
		this->impl_ptr = new impl();
	}
	catch (std::bad_alloc &e) {
		std::cerr << "Caught a bad_alloc exception in thread construction\n"; 
		throw e;
	}
	
	if(func == nullptr) {
		// delete impl_ptr BOOL;
		delete this->impl_ptr->getThreadOutOfScope();
		return;	// if the function is a NULL, then just return to init
	}
	// create the new context
	try {
		ucontext_t * ucontext_ptr = new ucontext_t();
		getcontext(ucontext_ptr);
		char *stack = new char [STACK_SIZE];
		ucontext_ptr->uc_stack.ss_sp = stack;
		ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
		ucontext_ptr->uc_stack.ss_flags = 0;
		ucontext_ptr->uc_link = nullptr;
		// fill the new context with 'modified' user code to run
		makecontext(ucontext_ptr, (void (*)()) enabler_function, 4 , this->impl_ptr, this->impl_ptr->getThreadOutOfScope(), func, args);
		// adds to ready queue
		assert(ucontext_ptr);
		cpu::self()->impl_ptr->push_back(ucontext_ptr);
		// signals any sleeping cpu's if necessary
		cpu::self()->impl_ptr->signalCPU();
		guard.exchange(false);
		cpu::self()->interrupt_enable();
	}
	catch (std::bad_alloc &e) {
		std::cerr << "Caught a bad_alloc exception during thread context creation\n";
		guard.exchange(false); // release the guard
		cpu::self()->interrupt_enable();
		throw e;
	}
}

thread::~thread() {
	assert_interrupts_enabled();
	cpu::self()->interrupt_disable();
	
	while(guard.exchange(true)) {}

	if(this->impl_ptr->getThreadExecutionDone()) {
		delete this->impl_ptr->getThreadOutOfScope();
	} else {
		this->impl_ptr->setThreadOutOfScope();
	}

	delete this->impl_ptr;

	guard.exchange(false);
	cpu::self()->interrupt_enable();
}

void thread::join() {
	// thread::join() pushes the currently running thread onto another thread's joined queue
	cpu::self()->interrupt_disable();

	while(guard.exchange(true)) {} // grab guard
	// only adds itself to join queue if thread execution has not finished
	if(!this->impl_ptr->getThreadExecutionDone()) {
		ucontext_t * next, * current;
		current = cpu::self()->impl_ptr->getRunningThread();
		if (!cpu::self()->impl_ptr->empty()) {
			next = cpu::self()->impl_ptr->front();
			cpu::self()->impl_ptr->pop_front();
			cpu::self()->impl_ptr->setRunningThread(next);
		} else {
			next = cpu::self()->impl_ptr->getCPU();
		}
		// push the currently running thread to another thread's joined waiting queue
		this->impl_ptr->pushToJoinQueue(current);

		swapcontext(current, next);
		
		// Makes sure invariants are held when we come back
		assert(guard);
		assert_interrupts_disabled();
	}

	guard.exchange(false);
	// and we should treat this like a noop
	cpu::self()->interrupt_enable();
}

// Lock down the cpu when switching back
void thread::yield() {
	cpu::self()->interrupt_disable();
	while(guard.exchange(true)) {} // grab guard
	if(!cpu::self()->impl_ptr->empty()) {
		ucontext_t * current = cpu::self()->impl_ptr->getRunningThread();
		// grab first person off the queue and swap contexts
		ucontext_t * next = cpu::self()->impl_ptr->front();
		cpu::self()->impl_ptr->pop_front();
		// add ourselves back to readyQ
		cpu::self()->impl_ptr->push_back(current);
		// tell a cpu that someone new is on readyQ
		cpu::self()->impl_ptr->signalCPU();
		assert(cpu::self()->impl_ptr->getRunningThread() != next);
		assert(current);
		cpu::self()->impl_ptr->setRunningThread(next);
		swapcontext(current, next);
		// Makes sure invariants are held when we come back
		assert(guard);
		assert_interrupts_disabled();
		cpu::self()->impl_ptr->exitQ_clear_all();
	}
	guard.exchange(false);
	cpu::self()->interrupt_enable();
}

