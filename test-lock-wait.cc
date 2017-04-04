// test that has multiple threads waiting for a single lock

#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int g = 0;

mutex mutex1;


void yielder(void * a) {
    intptr_t arg = (intptr_t) a;
    mutex1.lock();
    cout << "Hello there!" << endl;
    cout << "yielder " << g << " has let go of the lock!\n";
    ++g;
    mutex1.unlock();
}


void parent(void *a) {
    for(int i = 0; i < 20; i++) {    
        thread t1((thread_startfunc_t) yielder, (void *) (void *) nullptr);
    }

}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 15);
}
