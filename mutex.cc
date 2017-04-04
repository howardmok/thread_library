#include "mutex.h"
#include "cpu.h"
#include "mutex_impl.h"

mutex::mutex() {
    this->impl_ptr = new mutex::impl();
}

mutex::~mutex() {
    assert(mutex::impl_ptr);
    delete mutex::impl_ptr;
}

void mutex::lock() {
	cpu::self()->interrupt_disable();
    while(guard.exchange(true)) {} // grabs the guard
    this->impl_ptr->lockForMe();
    guard.exchange(false); // release the guard
    cpu::self()->interrupt_enable();
}

void mutex::unlock() {
	cpu::self()->interrupt_disable();
    while(guard.exchange(true)) {} // grabs the guard
    this->impl_ptr->unlockForMe();
    guard.exchange(false); // release the guard
    cpu::self()->interrupt_enable();
}