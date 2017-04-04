#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "thread.h"

using namespace std;

mutex mutex1;

void child(void *a) {
	cout << (char *) a << "\n";
	try {
		mutex1.unlock();
	} catch (...) {
		cout << "Hah. you tried to break me, but I caught you\n";
	}
}

void parent(void *a) {
    mutex1.lock();
    thread t1( (thread_startfunc_t) child, (void *) "Imma break you");
   	thread::yield();
}

int main()
{
	try {
		cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
	}
	catch(std::runtime_error& e) {
		cout << "Caught a runtime_error exception\n";
	}
	catch(...) {
		cout << "caught something bad";
	}

	return 0;
}