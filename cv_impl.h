#ifndef CV_IMPL_H
#define CV_IMPL_H

#include "cv.h"
#include "cpu_impl.h"
#include <deque>
#include <ucontext.h>
#include "cpu.h"

class cv::impl {
public:
	impl();
	~impl();
	void pushBackCvQ(ucontext_t *); 		//push to back of waiting queue for cv
	void popFrontCvQ(); 			//pop front of waiting queue for cv
	ucontext_t * frontCvQ(); 					//return front of waiting queue for cv
	bool emptyCvQ(); 				//check if waiting queue for cv is empty
private:
	std::deque<ucontext_t *> cvQ; 			//waiting queue for cv
};

#endif