#pragma once
#include "Mutex.h"
#include "Condition.h"
#include "noncopyable.h"
class CountDownLatch:noncopyable{
public:
	explicit CountDownLatch(int count);
	~CountDownLatch(){}
	void countDown();
	void wait();
private:
	MutexLock mutex_;
	Condition cond_;
	int count_;
};
