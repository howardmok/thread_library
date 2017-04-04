#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

mutex mutex1;
cv cv1;
int g = 0;

void summonZerglings(void *a) {
	cout << "I'm a zergling and i want the lock!!!\n";
	mutex1.lock();
	cout << "Zergling #" << g << " reporting for duty!\n";
	cv1.signal();
	cout << "I have informed the superiors that I got it!\nZergling #" << g << " out!\n";
	++g;
	mutex1.unlock();
}

void loop(void *a) {
	cout << "t1 hath arrived!!!\n";
    mutex1.lock();
    for(int i = 0; i < 5; ++i) {
		thread t1 ( (thread_startfunc_t) summonZerglings, (void *) nullptr);
    }
    cv1.wait(mutex1);
    cout << "Shhhh don't wake up the parentals!\n";
    cv1.signal();
    cout << "Oops... They're awake\n";
    cv1.wait(mutex1);

}

void parent(void *a) {
	cout << "calling in thread t1!!!\n";
	thread t1 ( (thread_startfunc_t) loop, (void *) "anotha one");
	mutex1.lock();
	cout << "parent took away the lock muahaha\nBut now it's bedtime.\n";
 	cv1.wait(mutex1);
 	cout << "it's 6am. parent is awake!!!\n";
}

int main()
{

	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);

	return 0;
}