#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int g = 0;

mutex mutex1;
cv cv1;
bool trigger = false;
int i = 0;
int counter = 0;
int runs = 0;
bool lastRun;

// test-signal-lock-unlock.cc tests 
// 1. cv signaling without the lock and no waiting mutexes 
// 2. cv signalling with waiting mutexes

void signaler(void *a) {
    char *id = (char *) a;
    cout << id << " is starting" << endl;
    for(int i = 0; i < 5; ++i) {
        cout << id << " is going to signal" << endl;
        cv1.signal();
    }
}

void locker(void *a) {
    mutex1.lock();
    int arg = g;
    g++;
    cout << "child " << arg << " got the lock. going to bed now" << endl;
    ++counter;
    if(counter == 5){
        cv1.broadcast();
    }
    cv1.signal();
    while(trigger == false){
        cv1.wait(mutex1);
    }
    ++runs;
    cout << "child " << arg << " is exiting" << endl;
    if(runs == 5){
        lastRun = true;
        cv1.broadcast();
    }
    mutex1.unlock();
}

void parent(void *a) {
    mutex1.lock();
    intptr_t arg = (intptr_t) a;

    // sig will signal 5 times without the lock with no one waiting
    thread sig( (thread_startfunc_t) signaler, (void *) "signaler");
    sig.join(); // signaller does nothing b/c no one is waiting
    cout << "joined has finished" << endl;
    for(;i < arg; ++i) {
        // create 5 threads that are waiting for a signal
        thread t1 ( (thread_startfunc_t) locker, (void *) nullptr);
    }
    while(counter != 5) {
        cv1.wait(mutex1);   
    }

    for(int i = 0; i < 5; i++){
        trigger = true;
        cv1.signal();
    }


    while(lastRun == false){
        cv1.wait(mutex1);
    }

    cout << "parent out. peace" << endl;
    mutex1.unlock();
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *) 5, false, false, 15);
}
