#include "Epoller.h"

Epoller::Epoller():epollFd_(epoll_create1(EPOLL_CLOEXEC)),events_(EPOLL_EVENTS){}

std::vector<Epoller::ChannelPtr> Epoller::poll(){
	//struct epoll_event events
	while(true){
		int count=epoll_wait(epollFd_,&*events_.begin(),events_.size(),EPOLLWAIT_TIME);
		std::vector<Epoller::ChannelPtr> ret;
		for(int i=0;i<count;i++){
			int fd=events_[i].data.fd;
			auto itr=channelPtrMap_.find(fd);
			if(itr!=channelPtrMap_.end()){
				itr->second->setRevents(events_[i].events);
				ret.push_back(channelPtrMap_[fd]);
			}
			else
				LOG<<"fd is invalid index";
		}
		if(ret.size()>0)
			return ret;
	}
	//return ret;
}
void Epoller::epoll_add(ChannelPtr chan,int timeout){
	int fd=chan->getFd();
	if(timeout>0){
		auto hService=chan->getHolder();
		if(hService){
			updateTimer(hService,timeout);
			/*hService->setTimer(timeout);
			timerQ.addTimer(hService->getTimer());*/
			httpServicePtrMap_[fd]=hService;
		}
		else
			LOG<<"channel does not have a httpservice";	
	}
	struct epoll_event event;
	event.events=chan->getEvents();
	event.data.fd=fd;
	chan->equalAndUpdateLastEvents();
	if(epoll_ctl(epollFd_,EPOLL_CTL_ADD,fd,&event)<0){
		LOG<<"epoll_ctl_add failed!";
		perror("epoll_add error");
		channelPtrMap_.erase(fd);
		httpServicePtrMap_.erase(fd);
	}
	channelPtrMap_[fd]=chan;	
}
void Epoller::epoll_del(ChannelPtr chan){
	int fd=chan->getFd();
	struct epoll_event event;
	event.events=chan->getLastEvents();
	event.data.fd=fd;
	if(epoll_ctl(epollFd_,EPOLL_CTL_DEL,fd,&event)<0){
		LOG<<"epoll_ctl_del failed!";
	}
	channelPtrMap_.erase(fd);
	httpServicePtrMap_.erase(fd);
}
void Epoller::updateTimer(HttpServicePtr httpService,int timeout){
	httpService->setTimer(timeout);
	timerQ.addTimer(httpService->getTimer());
}
void Epoller::epoll_mod(ChannelPtr chan,int timeout){
	if(timeout>0){
		auto hService=chan->getHolder();
		if(hService)
			updateTimer(hService,timeout);
	}
	int fd=chan->getFd(); 
	struct epoll_event event;
	event.events=chan->getEvents();
	event.data.fd=fd;
	if(epoll_ctl(epollFd_,EPOLL_CTL_MOD,fd,&event)<0){
		LOG<<"epoll_ctl_mod failed!";
		LOG<<"errno:"<<errno<<'\n';
		channelPtrMap_.erase(fd);
		httpServicePtrMap_.erase(fd);
	}	
}
