// this is a straightforward test that just yields between created threads

#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int g = 0;

mutex mutex1;
cv cv1;

void yielder2(void * a) {
    thread::yield();
    mutex1.lock();
    cout << "I'm going to sneak in :D" << endl;
    mutex1.unlock();
    thread::yield();
}

void yielder(void * a) {
    mutex1.lock();
    cout << "Hello there!" << endl;
    mutex1.unlock();
    thread::yield();

    cout << "I'm back!" << endl;

}


void parent(void *a) {
    for(int i = 0; i < 10; i++) {    
        thread t1((thread_startfunc_t) yielder, (void *) nullptr);
    }
    for(int i = 0; i < 3; i++){
        thread t2((thread_startfunc_t) yielder2, (void *) nullptr);
    }

}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}
