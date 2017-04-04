#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int g = 0; 
mutex mutex1;

void stubbornChild(void *a) {
	cout << "child wants the lock lock\n";
    mutex1.lock();
    cout << "I got the lock. now I'm never letting go.\nSincerely,\n#" << g++ << "\n";
}

void parent(void *a) {
	for(int i = 0; i < 10; ++i) {
		thread t1( (thread_startfunc_t) stubbornChild, (void *) nullptr);
	}    
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);

	return 0;
}