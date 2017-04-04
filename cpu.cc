#include "cpu.h"
#include "cpu_impl.h"
#include <ucontext.h>
#include <iostream>
#include <new>
#include <stdexcept>

void emptyFunc() {

}

static void cpu_instance (cpu::impl *i_ptr, thread_startfunc_t func, void * arg){
	assert_interrupts_disabled();
	assert(guard);
	if(func != nullptr) {
		guard.exchange(false);
		// if func is not a nullptr, then create a thread to initialize it
		cpu::self()->interrupt_enable();
		try {
			thread t(func, arg); // threads add their own contexts to ready queue
		}
		catch (std::runtime_error &e) {
			std::cerr << "Caught a runtime_error exception\n";
			throw e;
		}
		catch (std::bad_alloc &e) {
			std::cerr << "Caught a bad_alloc exception\n";
			throw e;
		}
		assert_interrupts_enabled();
		cpu::self()->interrupt_disable();
		while(guard.exchange(true)) {} // grab the guard
	}

	while(true) {
		ucontext_t *next = nullptr;
		assert_interrupts_disabled();
		assert(guard);
		// put cpu to sleep if readyQ empty
		while(i_ptr->empty()) {
			// adds cpu to sleeping queue
			assert(guard);
			i_ptr->sleeping_push_back(cpu::self());
			guard.exchange(false); // release the guard
			// suspends cpu
			cpu::self()->interrupt_enable_suspend();
			// disable interrupts once woken
			cpu::self()->interrupt_disable();
			while(guard.exchange(true)) {} // grab guard
		}
		assert(guard);

		next = i_ptr->front();
		i_ptr->pop_front();
		i_ptr->setRunningThread(next);

		assert_interrupts_disabled();
		swapcontext(i_ptr->getCPU(), next); 
		// Makes sure invariants are held when we come back
		assert(guard);	
		assert_interrupts_disabled();
		i_ptr->setRunningThread(nullptr);
		i_ptr->exitQ_clear_all();
	} // end while
}

void cpu::init(thread_startfunc_t func, void *arg) {
	assert_interrupts_disabled();
	interrupt_vector_table[cpu::TIMER] = thread::yield;
	interrupt_vector_table[cpu::IPI] = emptyFunc;

	while(guard.exchange(true)) {} // grab the guard

	try {
		this->impl_ptr = new cpu::impl();

		// thisCPUpointer is a pointer to the cpu
		ucontext_t * temp_cpu_pointer = new ucontext_t;
		getcontext(temp_cpu_pointer);

		// initialize the stack
		char *stack = new char [STACK_SIZE];
		temp_cpu_pointer->uc_stack.ss_sp = stack;
		temp_cpu_pointer->uc_stack.ss_size = STACK_SIZE;
		temp_cpu_pointer->uc_stack.ss_flags = 0;
		temp_cpu_pointer->uc_link = nullptr;

		this->impl_ptr->setCPU(temp_cpu_pointer);
		assert_interrupts_disabled();
		assert(guard);
		makecontext(temp_cpu_pointer, (void (*)()) cpu_instance, 3 , this->impl_ptr, func, arg);
		setcontext(temp_cpu_pointer);

		assert(false);	// should never hit this point
	}
	catch(std::bad_alloc &e) {
		guard.exchange(false);
		cpu::self()->interrupt_enable();
		std::cerr << "Caught a bad_alloc exception\n";
		throw e;
	}
}