#pragma once
#include "base/noncopyable.h"
#include "EventLoopThread.h"
#include <vector>
#include <memory>
class EventLoopThreadPool:noncopyable{
public:
	EventLoopThreadPool(EventLoop*,int);
	~EventLoopThreadPool(){};
	void start();
	EventLoop* getNextLoop();
private:
	std::vector<std::unique_ptr<EventLoopThread>> loopThreads_;
	std::vector<EventLoop* > loops_;
	EventLoop* baseLoop_; 
	int numThreads_;
	int next_;
	bool started_;
};
