#ifndef CPU_IMPL_H
#define CPU_IMPL_H

#include "cpu.h"
#include "mutex.h"
#include "cv.h"
#include <ucontext.h>
#include <deque>
#include <cassert>

class cpu::impl {
public:
	impl();
	~impl();
	/*				  *
	 * readyQ methods *
	 *				  */
	bool empty();
	ucontext_t * front();
	void pop_front();
	void push_back(ucontext_t *);
	/* 					 *
	 * sleepingQ methods *
	 *					 */
	void signalCPU();
	void sleeping_push_back(cpu *);
	/*				 *
	 * exitQ methods *
	 *				 */
	void exitQ_push_back(ucontext_t *);
	void exitQ_clear_all();
	/*					 *
	 * cpu state methods *
	 * 					 */
	void setCPU(ucontext_t *);
	ucontext_t * getCPU();
	void setRunningThread(ucontext_t *);
	ucontext_t * getRunningThread();
private:
	static std::deque<ucontext_t *> readyQ;
	static std::deque<cpu *> sleepingCPU;
	static std::deque<ucontext_t *> exitQ;
	ucontext_t * thisCPUpointer;
	ucontext_t * currentlyRunningThread;

};	

#endif