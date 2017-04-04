#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

void numberTwo(void * a) {
    intptr_t arg = (intptr_t) a;
    cout << "I'm going to sneak in :D\n";
    thread::yield();
    cout << "They caught me.\n";
    thread::yield();
    cout << "second one #" << arg << " out.\n";
}

void numberOne(void * a) {
    intptr_t arg = (intptr_t) a;
    cout << "Hello there!\n";
    thread::yield();
    cout << "I'm back!\n";
    thread::yield();
    cout << "Is the other guy gone yet?\n";
    thread::yield();
    cout << "I'm out -first one #" << arg << "\n";
}


void parent(void *a) {
    intptr_t t = 0;
    for(int i = 0; i < 25; i++) {
        thread t1((thread_startfunc_t) numberOne, (void *) t);
        ++t;
        thread t2((thread_startfunc_t) numberTwo, (void *) t);
        ++t;
    }
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}
