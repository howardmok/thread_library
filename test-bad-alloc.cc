#include <iostream>
#include <cstdlib>
#include "thread.h"
#include <sys/resource.h>

using namespace std;

void child(void *a) {
	cout << (char *) a << "\n";;
}

void parent(void *a) {
    while(true) {
    	try {
    		thread t1 ( (thread_startfunc_t) child, (void *) "anotha one");
    	} catch (...) {
    		cout << "Badboy\n";
    		break;
    	}
    }
    cout << "we caught them bad boys\n";
}

int main() {
	int memory_limit = 100000000;
	struct rlimit rlim;
	rlim.rlim_cur = memory_limit;
	rlim.rlim_max = memory_limit;
	setrlimit(RLIMIT_AS, &rlim);

	try {
		cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
	}
	catch (bad_alloc &e) {
		cout << "Caught a bad_alloc exception.\n";
	}

	return 0;
}