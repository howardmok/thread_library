// calls cpu boot for a uniprocessor with no interrupts
// essentially checks that our init function works

#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

void printThings(void *a) {
	char * id = (char *) a;
	cout << id << " hello" << endl;
	cout << id << " sometimes I like to party" << endl;
	cout << id << " a little party never killed nobody" << endl;
	cout << id << " #fergie" << endl;
}

void thirdGen(void *a) {
	char * id = (char *) a;
	cout << id << " started" << endl;
	thread t1( (thread_startfunc_t) printThings, (void *) "I am baby");
	cout << id << " created child" << endl;
	cout << id << " joined to child" << endl;
	t1.join();
	cout << id << " finished join to child" << endl;
	cout << id << " ended" << endl;
}

void secondGen(void *a) {
	char * id = (char *) a;
	cout << id << " started" << endl;
	thread t1( (thread_startfunc_t) printThings, (void *) "I am second oldest");
	cout << id << " created child" << endl;
	cout << id << " joined to child" << endl;
	t1.join();
	cout << id << " finished join to child" << endl;
	cout << id << " ended" << endl;
}

void firstGen(void *a) {
	char * id = (char *) a;
	cout << id << " started" << endl;
	thread t1( (thread_startfunc_t) printThings, (void *) "I am oldest");
	cout << id << " created child" << endl;
	cout << id << " ended" << endl;
}

void parent(void * a) {
	cout << "parent called" << endl;
	thread g1( (thread_startfunc_t) firstGen, (void *) "firstGen");
	thread g2( (thread_startfunc_t) secondGen, (void *) "secondGen");
	thread g3( (thread_startfunc_t) thirdGen, (void *) "thirdGen");
	
	cout << "joining to firstGen child" << endl;
	g1.join();
	cout << "finished join to first child" << endl;
	
	cout << "joining to secondGen child" << endl;
	g2.join();
	cout << "finished join to second child" << endl;
	
	cout << "joining to thirdGen child" << endl;
	g3.join();
	cout << "finished join to third child" << endl;
	
	cout << "joining to firstGen child" << endl;
	g1.join();
	cout << "finished join to firstGen child" << endl;

	cout << "parent is finished" << endl;
}

int main() {
	// static void boot(unsigned int num_cpus, thread_startfunc_t func, void *arg, bool async, bool sync, unsigned int random_seed);
	cpu::boot(15, parent, nullptr, false,false,0);
}