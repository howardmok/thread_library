#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

cv cv1;
mutex mutex1;

void paddles(void *a) {
	char *args = (char *) a;
	cout << args << " is alive!\n";
	mutex1.lock();
	cout << args << " got the lock!\n";
	for(int i = 0; i < 10; ++i) {
		cout << args << " is passing the lock!\n";
		cv1.signal();
		cv1.wait(mutex1);
	}
	cout << args << " out.\n";
}

void parent(void *args) {
	mutex1.lock();
	thread t1 ( (thread_startfunc_t) paddles, (void *) "ping");
	thread t2 ( (thread_startfunc_t) paddles, (void *) "pong");
    mutex1.unlock();
	t1.join();
	t2.join();

    cout << "I should be printing out last!!!\n";
}

int main() {
	cout << "Calling CPU boot\n";
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
	return 0;
}