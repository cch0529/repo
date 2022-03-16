#pragma once
#include "base/noncopyable.h"
#include <memory>
#include "Channel.h"
#include <unordered_map>
#include "HttpService.h" 
#include <sys/epoll.h>
#include <vector>
#include "Timer.h"
#include <stdio.h>
#define EPOLL_EVENTS 4096
#define EPOLLWAIT_TIME 10000

class Epoller:noncopyable{
public:
	typedef std::shared_ptr<Channel> ChannelPtr;
	typedef std::shared_ptr<HttpService> HttpServicePtr;
	Epoller();
	~Epoller(){};
	std::vector<ChannelPtr> poll();
	void epoll_add(ChannelPtr chan,int timeout);
	void epoll_del(ChannelPtr chan);
	void epoll_mod(ChannelPtr chan,int timeout);
	void updateTimer(HttpServicePtr httpService,int timeout);
	int getEpollFd(){ return epollFd_;}
	void handleExpiredTimers(){ timerQ.handleExpiredTimer(); }
private:
	int epollFd_;
	std::vector<struct epoll_event> events_;
	std::unordered_map<int,ChannelPtr> channelPtrMap_;
	std::unordered_map<int,HttpServicePtr> httpServicePtrMap_;
	TimerQueue timerQ;
};
