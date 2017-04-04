#include "cv.h"
#include "cpu.h"
#include "cv_impl.h"
#include "mutex_impl.h"

cv::cv() {
    this->impl_ptr = new cv::impl();
}

cv::~cv() {
    assert(this->impl_ptr);
    delete this->impl_ptr;
}

// Atomically unlocks lock, adds thread to waiting set, sleeps
void cv::wait(mutex& lock) {
    cpu::self()->interrupt_disable();        

    while(guard.exchange(true)) {} // grab the guard;
    lock.impl_ptr->unlockForMe();
    ucontext_t * current = cpu::self()->impl_ptr->getRunningThread();
    this->impl_ptr->pushBackCvQ(current); //pushes it onto queue waiting for cv
    if(cpu::self()->impl_ptr->empty() == true) {
        cpu::self()->impl_ptr->setRunningThread(nullptr);
        swapcontext(current, cpu::self()->impl_ptr->getCPU());
    } else {
        ucontext_t * next = cpu::self()->impl_ptr->front();
        cpu::self()->impl_ptr->pop_front();
        cpu::self()->impl_ptr->setRunningThread(next);
        swapcontext(current, next);
    }
    assert(guard);
    assert_interrupts_disabled();
    lock.impl_ptr->lockForMe();
    guard.exchange(false); // release the guard
    cpu::self()->interrupt_enable();
}

void cv::signal() {
    cpu::self()->interrupt_disable();
    while(guard.exchange(true)) {} // grab the guard
    if(!this->impl_ptr->emptyCvQ()) { // signal first thread if it exists
        ucontext_t *waitingThread = this->impl_ptr->frontCvQ();
        this->impl_ptr->popFrontCvQ(); //pop off front member on waiting queue for cv
        cpu::self()->impl_ptr->push_back(waitingThread); //put it back on ready queue
    }
    guard.exchange(false); // release the guard
    cpu::self()->interrupt_enable();
}

void cv::broadcast() {
    cpu::self()->interrupt_disable();
    while(guard.exchange(true)) {} // grab the guard
    while(!this->impl_ptr->emptyCvQ()) {
        ucontext_t *waitingThread = this->impl_ptr->frontCvQ();
        this->impl_ptr->popFrontCvQ(); //pop off front member on waiting queue for cv
        cpu::self()->impl_ptr->push_back(waitingThread); //put it back on ready queue
    }
    guard.exchange(false); // release the guard
    cpu::self()->interrupt_enable();
}