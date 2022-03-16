#include "EventLoopThread.h"

EventLoopThread::EventLoopThread():mutex_(),cond_(mutex_),
				thread_(std::bind(&EventLoopThread::threadFunc,this)){}
EventLoopThread::~EventLoopThread(){
	if(loop_!=NULL){
		loop_->quit();
		thread_.join();
	}
}
EventLoop* EventLoopThread::startLoop(){
	thread_.start();
	{
		MutexLockGuard lock(mutex_);
		while(!loop_)
			cond_.wait();
	}
	return loop_;
}
void EventLoopThread::threadFunc(){
	EventLoop loop;
	{
		MutexLockGuard lock(mutex_);
		loop_=&loop;
		cond_.notify();
	}	
	loop.loop();
	loop_=NULL;
}
