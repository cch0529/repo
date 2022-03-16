#include "Timer.h"

Timer::Timer(TimerCallBack cb,int timeout):cb_(cb),canceled_(false),usedUp_(false){
	struct timeval now;
	gettimeofday(&now,NULL);
	expiration_=(now.tv_sec%10000)*1000+now.tv_usec/1000+timeout;
}
bool Timer::isUsedUp(){
	if(usedUp_)
		return true;
	struct timeval now;
	gettimeofday(&now,NULL);
	size_t curTime=(now.tv_sec%10000)*1000+now.tv_usec/1000;
	//LOG<<"CurTime"<<curTime;
	if(curTime>=expiration_){
		usedUp_=true;
		return true;
	}
	return false;
}
void Timer::run(){
	//printf("in timer run()\n");
	if(!canceled_){
		//printf("ready to run cb()\n");
		cb_();
	}
	
		//printf("dont run cb()\n");
}
void Timer::update(int timeout){
		struct timeval now;
		gettimeofday(&now,NULL);
		expiration_=(now.tv_sec%10000)*1000+now.tv_usec*1000+timeout;
}
void TimerQueue::addTimer(TimerPtr timer){
	queue_.push(timer);
}
void TimerQueue::handleExpiredTimer(){
	//printf("in handleEcpiretimer\n");
	while(!queue_.empty()){
		TimerPtr tmpTimer=queue_.top();
		if(tmpTimer->isUsedUp()){
			//printf("time out\n");
			tmpTimer->run();
			queue_.pop();
		}
		else
			break;
	}
}
