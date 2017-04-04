#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

mutex mutex1;
cv cv1;

void loop(void *a) {
    cv1.signal();
}

void parent(void *a) {

    mutex1.lock();

    thread t1 ( (thread_startfunc_t) loop, (void *) "child thread");

}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);

    return 0;
}
