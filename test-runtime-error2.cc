#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "thread.h"

using namespace std;

mutex mutex1;

void parent(void *a) {
	try {
	    mutex1.unlock();
	} catch(...) {
		cout << "Still trying to unlock something you don't own smh\n";
	}
	try {
	    mutex1.unlock();
	} catch(...) {
		cout << "AGAIN WITH THE TRYING TO UNLOCK SOMETHING YOU DON'T OWN? WHY...\n";
	}
}

int main()
{
	try {
		cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
	}
	catch (...) {
		cout << "Caught a runtime_error exception.\n";
	}

	return 0;
}