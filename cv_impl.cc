#include "cv_impl.h"

cv::impl::impl(){

}

cv::impl::~impl(){

}

void cv::impl::pushBackCvQ(ucontext_t *t) {
	assert_interrupts_disabled();
	assert(guard);
	cv::impl::cvQ.push_back(t);
}

void cv::impl::popFrontCvQ() {
	assert_interrupts_disabled();
	assert(guard);
	cv::impl::cvQ.pop_front();
}

ucontext_t * cv::impl::frontCvQ() {
	assert_interrupts_disabled();
	assert(guard);
	return cv::impl::cvQ.front();
}

bool cv::impl::emptyCvQ() {
	assert_interrupts_disabled();
	assert(guard);
	return cv::impl::cvQ.empty();
}
