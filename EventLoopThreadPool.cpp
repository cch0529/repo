#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop,int numThreads):baseLoop_(loop),
									started_(false),
									numThreads_(numThreads){
	if(numThreads_<=0){
		LOG<<"numThreads<=0";
		abort();
	}
	
}
void EventLoopThreadPool::start(){
	baseLoop_->assertInLoopThread();
	assert(!started_);
	started_=true;
	for(int i=0;i<numThreads_;++i){
		EventLoopThread* tmpLoopThread=new EventLoopThread();
		loops_.push_back(tmpLoopThread->startLoop());
		loopThreads_.push_back(std::unique_ptr<EventLoopThread>(tmpLoopThread));
		
	}
}
EventLoop* EventLoopThreadPool::getNextLoop(){
	baseLoop_->assertInLoopThread();
	assert(started_);
	EventLoop* loop=baseLoop_;
	if(!loops_.empty()){
		loop=loops_[next_];
		next_=(next_+1)%numThreads_;
	}
	return loop;	
}
