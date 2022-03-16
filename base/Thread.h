#pragma once
#include <pthread.h>
#include "noncopyable.h"
#include <functional>
#include "CurrentThread.h"
#include "CountDownLatch.h"
#include <sys/types.h>
class Thread:noncopyable{
public:	
	typedef std::function<void()> ThreadFunc;
	Thread(ThreadFunc);
	~Thread(){}
	void start();
//	void* startThread(void* obj);
	int join();
private:
	//typedef std::function<void()> ThreadFunc;
	ThreadFunc func_;
	pid_t tid_;
	pthread_t threadId_;
	CountDownLatch latch_;
	bool started_;
	bool joined_;
};
