#pragma once
#include "base/noncopyable.h"
#include "EventLoop.h"
#include "base/Thread.h"
#include "base/Mutex.h"
class EventLoopThread:noncopyable{
public:
	EventLoopThread();
	~EventLoopThread();
	EventLoop* startLoop();
private:
	void threadFunc();
	EventLoop* loop_;
	Thread thread_;
	MutexLock mutex_;
	Condition cond_;
};
