#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int g = 0;

mutex mutex1;
cv cv1;

void signaler(void *a) {
    char *id = (char *) a;
    cout << id << " is starting" << endl;
    for(int i = 0; i < 40; ++i) {
        mutex1.lock();
        cout << id << " is going to signal" << endl;
        cv1.signal();
        mutex1.unlock();
    }
}

void locker(void *a) {
    mutex1.lock();
    int arg = g;
    g++;
    cout << "child " << arg << " got the lock. going to bed now" << endl;
    cv1.wait(mutex1);
    cout << "child " << arg << "stole the lock" << endl;
    mutex1.unlock();
}

void parent(void *a) {
    intptr_t arg = (intptr_t) a;

    mutex1.lock();
    thread sig( (thread_startfunc_t) signaler, (void *) "signaler");
    for(int i = 0; i < arg; ++i) {
        int q = i;
        thread t1 ( (thread_startfunc_t) locker, (void *) &q);
    }
    mutex1.unlock();
    cout << "parent out. peace" << endl;
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *) 30, false, false, 0);
}
