#include "Channel.h"
#include "EventLoop.h"
Channel::Channel(EventLoop* loop):loop_(loop),events_(0),revents_(0),fd_(0){}
Channel::Channel(EventLoop* loop,int fd):loop_(loop),fd_(fd),events_(0),revents_(0){}

void Channel::handleEvent(){
	//LOG<<"in handleEvent";
	events_=0;
	if(revents_==0)
		return;
	if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
		events_=0;
		return ;
	}
	if(revents_ & EPOLLERR){
		if(errorCallBack_)
			errorCallBack_();
		events_=0;
		return;
	}
	if(revents_ & (EPOLLIN | EPOLLPRI)){
		if(readCallBack_){
			//LOG<<"call readCallBack";
			readCallBack_();
		}
	}
	if(revents_ & (EPOLLOUT))
		if(writeCallBack_)
			writeCallBack_();
	if(holder_.lock())
		loop_->updatePoller(shared_from_this());
}
