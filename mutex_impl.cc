#include "mutex_impl.h"

mutex::impl::impl() : status(false), lockHolder(nullptr){
	//ctor
}

mutex::impl::~impl(){
	//dtor
}

// REQUIRED: Guard must be held 
//           Interrupts are disabled
// EFFECTS: Tries to acquire a lock for a given thread.
//          If lock is held, put the thread on a waiting queue
//          and swap to the nexy ready thread. If no ready threads
//          available, then swap to CPU. 
void mutex::impl::lockForMe() {
	assert_interrupts_disabled();
	assert(guard);
    if(!mutex::impl::status) { // lock is free
        mutex::impl::status = true; // lock is busy
        mutex::impl::lockHolder = cpu::self()->impl_ptr->getRunningThread();
    } else {
        // lock is currently held by some other thread 
        // add thread to waiting queue for lock, switch to next
        ucontext_t * next = nullptr;
        ucontext_t *current = cpu::self()->impl_ptr->getRunningThread();
        mutex::impl::mutexQ.push_back(current); // push the currently running thread onto the mutex waiting queue
        if(cpu::self()->impl_ptr->empty()) {
            // return control to CPU if readyQ is empty
        	next = cpu::self()->impl_ptr->getCPU();
        	cpu::self()->impl_ptr->setRunningThread(nullptr);
        } else { 
            // swap to next thread on the readyQ   
            next = cpu::self()->impl_ptr->front();
            cpu::self()->impl_ptr->pop_front();
	        cpu::self()->impl_ptr->setRunningThread(next);
        }
        swapcontext(current, next); // switch to the next available thread to run

        // At this point, some other thread has signaled us to run and 
        // have handed us the lock. 
        assert(mutex::impl::status);
        assert(cpu::self()->impl_ptr->getRunningThread() == mutex::impl::lockHolder);
        mutex::impl::lockHolder = cpu::self()->impl_ptr->getRunningThread();
    }
    // Make sure invariants are held when we come back
    assert(guard);
    assert_interrupts_disabled();
}

void mutex::impl::unlockForMe() {
	assert_interrupts_disabled();
	assert(guard);
    // only person that grabbed the lock can unlock it
    if(lockHolder != cpu::self()->impl_ptr->getRunningThread()) {
        std::cerr << "Unlocked something you don't own\n";
        guard.exchange(false);
        cpu::self()->interrupt_enable();
        throw std::runtime_error("Unlocked something you don't own");
    } else {
    	mutex::impl::status = false;
        mutex::impl::lockHolder = nullptr;

        if(!mutexQ.empty()) { 
            // if any thread is waiting for lock
        	ucontext_t *waiting = mutexQ.front(); //move thread
        	mutexQ.pop_front();
            cpu::self()->impl_ptr->push_back(waiting); //ready queue
            mutex::impl::status = true;
            mutex::impl::lockHolder = waiting;
        }
    }
	assert(guard);
    assert_interrupts_disabled();
}