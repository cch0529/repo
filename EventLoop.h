#pragma once
#include "base/noncopyable.h"
#include "sys/types.h"
#include "base/CurrentThread.h"
#include "Epoller.h"
#include <memory>
#include <sys/eventfd.h>
#include <functional>
//#include "Channel.h"
#include <vector>
#include "base/Mutex.h"
#include <unistd.h>
//#include "Epoller.h"
class Epoller;
class Channel;
class EventLoop:noncopyable{
public:
	typedef std::function<void()> Functor;
	EventLoop();
	~EventLoop();
	void loop();
	void quit(){
		quit_=true; 
		if(!isInLoopThread())
			wakeUp();
	}
	bool isInLoopThread(){ return threadId_ == CurrentThread::tid();}
	void assertInLoopThread();
	void removeFromPoller(std::shared_ptr<Channel>);
	void addToPoller(std::shared_ptr<Channel> chan,int timeout=0);
	void updatePoller(std::shared_ptr<Channel> chan,int timeout=0);
	void wakeUp();
	void runInLoop(Functor);
	void queueInLoop(Functor);
private:
	void handleRead();
	void abortNotInLoopThread();
//	int createEvent();
	MutexLock mutex_;
	const pid_t threadId_;
	bool looping_;
	bool quit_;
	int eventFd_;
	std::vector<Functor> pendingFunctors_;
	std::unique_ptr<Epoller> poller_;
	void doPendingFunctors();
	std::shared_ptr<Channel> wakeUpChannel_;
	bool callingPendingFunctors_;
};
