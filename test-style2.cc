#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

mutex mutex1;
mutex mutex2;
cv cv1;

void loop(void *a) {
	cout << "Woohoo! I hath been birthed into existence!\n";
    mutex2.lock();
    cout << "I got that lock thingy. Lock #2 is mine!\n";
    cout << "Now it's time to go to bed. Releasing mutex2 now!\n";
    cv1.wait(mutex2);
    cout << "I have been awakened! Mutex2 is mine again!\n";

}

void parent(void *a) {
	cout << "Making a new thread\n";
	thread t1 ( (thread_startfunc_t) loop, (void *) "anotha one");
	cout << "Made the thread\n";
	cout << "about to grab the lock\n";
    mutex1.lock();
    cout << "woohoo I got the lock.\n";
    cout << "now it's time to go to bed. releasing mutex1 now\n";
 	cv1.wait(mutex1);
 	cout << "I have been awakened! Mutex1 is mine again!\n";
}

int main()
{

	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);

	return 0;
}