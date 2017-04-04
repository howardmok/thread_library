#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int g = 0;

mutex mutex1;
cv cv1;

bool trigger = false;

void unlocker(void * args) {
	char *id = (char *) args;
	cout << "ALL MAY PASS because " << id << " has ran" << endl;
	trigger = true;
	cv1.broadcast();
	cout << "ok it really did run" << endl;
}

void loop(void *a) {
	mutex1.lock();
	while (trigger == false) {
		cv1.wait(mutex1);
	}
    char *id = (char *) a;
    int i;

    cout << "loop called with id " << id << endl;

    for (i=0; i<5; i++, g++) {
	cout << id << ":\t" << i << "\t" << g << endl;
    }
    cout << id << ":\t" << i << "\t" << g << endl;
    mutex1.unlock();
}

void parent(void *a) {
    for(int i = 0; i < 5; i++) {
    	thread t1 ( (thread_startfunc_t) loop, (void *) "child thread");
   	}

    mutex1.lock();
    thread broadcaster( (thread_startfunc_t) unlocker, (void *) "unlocker" );
    mutex1.unlock();
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}
