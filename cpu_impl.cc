#include "cpu_impl.h"

// #include <stdexcept>

std::deque<ucontext_t *> cpu::impl::readyQ;
std::deque<cpu *> cpu::impl::sleepingCPU;
std::deque<ucontext_t *> cpu::impl::exitQ;

cpu::impl::impl() {
	//ctor
	cpu::impl::thisCPUpointer = nullptr;
	cpu::impl::currentlyRunningThread = nullptr;
}


cpu::impl::~impl() {
	//dtor
}

/*				  *
 * readyQ methods *
 *				  */
bool cpu::impl::empty() {
	assert(guard);
	return cpu::impl::readyQ.empty();
}

ucontext_t * cpu::impl::front() {
	assert(guard);
	assert(!cpu::impl::readyQ.empty());
	return cpu::impl::readyQ.front();
}

void cpu::impl::pop_front() {
	assert(guard);
	assert(!cpu::impl::readyQ.empty());
	cpu::impl::readyQ.pop_front();
}

void cpu::impl::push_back(ucontext_t * temp) {
	assert(guard);
	assert(temp != nullptr);
	cpu::impl::readyQ.push_back(temp);
}

/* 					 *
 * sleepingQ methods *
 *					 */
void cpu::impl::signalCPU() {
	assert_interrupts_disabled();
	assert(guard);
	assert_interrupts_disabled();
	if(!cpu::impl::sleepingCPU.empty()) {
		cpu *sleepingCPU = cpu::impl::sleepingCPU.front();
		cpu::impl::sleepingCPU.pop_front();
		sleepingCPU->interrupt_send();
		// assert(false);
	}
}

void cpu::impl::sleeping_push_back(cpu * sCPU) {
	assert(guard);
	cpu::impl::sleepingCPU.push_back(sCPU);
}

/*				 *
 * exitQ methods *
 *				 */
void cpu::impl::exitQ_push_back(ucontext_t * out) {
	assert(guard);
	cpu::impl::exitQ.push_back(out);
}

void cpu::impl::exitQ_clear_all() {
	assert(guard);
	assert_interrupts_disabled();
	while(!exitQ.empty()) {
		if(exitQ.front()->uc_stack.ss_sp) {
			delete[] (char *) exitQ.front()->uc_stack.ss_sp;
			exitQ.front()->uc_stack.ss_sp = nullptr;
			delete exitQ.front();
			exitQ.front() = nullptr;
			exitQ.pop_front();
		}
	}
}

/*					 *
 * cpu state methods *
 * 					 */
void cpu::impl::setCPU(ucontext_t * cpuPointer) {
	assert(cpuPointer);
	cpu::impl::thisCPUpointer = cpuPointer;
	assert(cpu::impl::thisCPUpointer);
}

ucontext_t * cpu::impl::getCPU() {
	return cpu::impl::thisCPUpointer;
}

void cpu::impl::setRunningThread(ucontext_t * tempRunningThread) {
	cpu::impl::currentlyRunningThread = tempRunningThread;
}

ucontext_t * cpu::impl::getRunningThread() {
	return cpu::impl::currentlyRunningThread;
}