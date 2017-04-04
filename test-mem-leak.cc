#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

void print_out(void *a) {
	intptr_t arg = (intptr_t) a;
	cout << "child " << arg << endl;
}

void parent(void *a) {
	intptr_t arg = (intptr_t) a;
	for(int i = 0; i < arg; ++i) {
		intptr_t temp = 0;
		try{
			thread t1((thread_startfunc_t) print_out, (void *) temp);
			temp = i + arg;
			thread t2((thread_startfunc_t) print_out, (void *) temp);
			temp += arg;
			thread t3((thread_startfunc_t) print_out, (void *) temp);
		} catch(...) {
			cout << "well, something is wrong with your code\n";
		}
	}
	cout << "you done good\n";
}

int main() {
	try {
		cpu::boot(1, (thread_startfunc_t) parent, (void *) 1000, false, false, 0);
	} catch(...) {
		cerr << "Welp, something broke there. Might want to fix that\n";
	}
	return 0;
}