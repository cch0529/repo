#pragma once
#include "./base/noncopyable.h"
#include <functional>
//#include "EventLoop.h"
#include "base/Logging.h"
#include <sys/epoll.h>
#include <memory>
//#include "HttpService.h"
class EventLoop;
class HttpService;
class Channel:noncopyable,
		public std::enable_shared_from_this<Channel>{
public:
	typedef std::function<void()> CallBack;
	Channel(EventLoop*);
	Channel(EventLoop*,int );
	~Channel(){
	//	LOG<<"CHANNEL DISCONSTRUCT"<<'\n';
	}
	/*try not to use right value callback*/
	void setReadCallBack(CallBack cb){ readCallBack_=cb;}
	void setWriteCallBack(CallBack cb){writeCallBack_=cb;}
	void setErrorCallBack(CallBack cb){errorCallBack_=cb;}
	void setFd(int fd){ fd_=fd; }
	void setRevents(uint32_t events){ revents_=events; }
	void handleEvent();
	uint32_t getLastEvents(){ return lastEvents_; }
	int getFd(){ return fd_; }
	void setEvents(uint32_t events){ events_=events; }
	uint32_t getEvents(){ return events_; }
	std::shared_ptr<HttpService> getHolder(){
		return holder_.lock();
	}
	void setHolder(std::shared_ptr<HttpService> holder){ holder_=holder; }
	bool equalAndUpdateLastEvents(){
		bool ret=lastEvents_==events_;
		lastEvents_=events_;
		return ret;
	}
	//std::shared_ptr<Epoller> getPoller(){ return poller_.lock(); }
private:
	//typedef function<void()> CallBack;
	CallBack readCallBack_;
	CallBack writeCallBack_;
	CallBack errorCallBack_;
	int fd_;
	EventLoop* loop_;
	uint32_t events_;
	uint32_t revents_;
	uint32_t lastEvents_;
	std::weak_ptr<HttpService> holder_;
	//std::weak_ptr<Epoller> poller_;	
};
