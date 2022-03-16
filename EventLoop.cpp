#include<poll.h>
#include "EventLoop.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Epoller.h"
__thread EventLoop* t_currentThreadLoop=0;
int createEvent(){
	int fd=eventfd(0,EFD_NONBLOCK | EFD_CLOEXEC);
	if(eventfd<0){
		LOG<<"failed in createEvent";
		abort();
	}	
	return fd;
}
EventLoop::EventLoop():looping_(false),
			threadId_(CurrentThread::tid()),
			poller_(new Epoller()),
			quit_(false),
			callingPendingFunctors_(false),
			eventFd_(createEvent()),
			wakeUpChannel_(new Channel(this,eventFd_)){
	if(t_currentThreadLoop){
		LOG<<"there is already a eventloop in the thread";
		
	}
	else
		t_currentThreadLoop=this;
	wakeUpChannel_->setEvents(EPOLLIN | EPOLLET);
	wakeUpChannel_->setReadCallBack(std::bind(&EventLoop::handleRead,this));
	poller_->epoll_add(wakeUpChannel_,0);
} 
EventLoop::~EventLoop(){
	assert(!looping_);
	close(eventFd_);
	t_currentThreadLoop=NULL;
}
void EventLoop::loop(){
	assert(!looping_);
	assertInLoopThread();
	looping_=true;
	while(!quit_){
		std::vector<std::shared_ptr<Channel>> channels=poller_->poll();
		for(auto chan:channels){
			chan->handleEvent();
		}
		poller_->handleExpiredTimers();
		doPendingFunctors();
		//poller_.handleExpiredTimers();
	}	
	//printf("stop looping");
	looping_=false;
}
void EventLoop::abortNotInLoopThread(){
	printf("eventloop_id=%d,currentthread_id=%d",threadId_,CurrentThread::tid());
	exit(0);
}

void EventLoop::assertInLoopThread(){
	if(!isInLoopThread()){
		abortNotInLoopThread();
	}
}
void EventLoop::removeFromPoller(std::shared_ptr<Channel> chan){
	poller_->epoll_del(chan);
}
void EventLoop::addToPoller(std::shared_ptr<Channel> chan,int timeout){
	poller_->epoll_add(chan,timeout);
}
void EventLoop::updatePoller(std::shared_ptr<Channel> chan,int timeout){
	poller_->epoll_mod(chan,timeout);
}
void EventLoop::doPendingFunctors(){
	callingPendingFunctors_=true;
	std::vector<Functor> functors;
	{
		MutexLockGuard lock(mutex_);
		functors.swap(pendingFunctors_);
	}	
	for(const auto& func:functors){
		func();
	}
	callingPendingFunctors_=false;
}
/*int createEvent(){
	int fd=eventfd(0,EFD_NONBLOCK | EFD_CLOEXEC);
	if(eventfd<0){
		LOG<<"failed in createEvent";
		abort();
	}	
	return fd;
}*/
void EventLoop::handleRead(){
	uint64_t data=0;
	int n;
	if(n=read(eventFd_,&data,8)!=8){
		LOG<<"failed int read event ,read "<<n<<"bytes from event";
	}
	wakeUpChannel_->setEvents(EPOLLIN |EPOLLET);
	
}
void EventLoop::wakeUp(){
	uint64_t data=1;
	int n=0;
	if(n=write(eventFd_,&data,8)!=8){
		LOG<<"failed in write event,write"<<n<<"bytes";
	}
}
void EventLoop::queueInLoop(Functor cb){
	{
		MutexLockGuard lock(mutex_);
		pendingFunctors_.push_back(cb);	
	}
	if(!isInLoopThread() || callingPendingFunctors_)
		wakeUp();
}
void EventLoop::runInLoop(Functor cb){
	if(isInLoopThread())
		cb();
	else
		queueInLoop(cb);
}
