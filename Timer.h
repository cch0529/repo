#pragma once
#include "base/noncopyable.h"
#include <memory>
#include <functional>
#include <queue>
#include <vector>
#include <sys/time.h>
#include "base/Logging.h"
class Timer:noncopyable{
public:
	typedef std::function<void()> TimerCallBack;
	Timer(TimerCallBack,int);
	~Timer(){}	
	void run();
	void setCanceled(){ canceled_=true; }
	bool isUsedUp();
	//void setUsedUp(){ usedUp_=true; }
	void update(int);
	size_t getExpiration(){ return expiration_; }	
private:
	TimerCallBack cb_;
	size_t expiration_;
	bool canceled_;
	bool usedUp_;
};
struct TimerCmp{
	bool operator()(std::shared_ptr<Timer>& timer1,std::shared_ptr<Timer>& timer2){
		return timer1->getExpiration()>timer2->getExpiration();
	}
};
class TimerQueue:noncopyable{
public:
	typedef std::shared_ptr<Timer> TimerPtr;
	TimerQueue(){}
	~TimerQueue(){}
	void addTimer(TimerPtr);
	void handleExpiredTimer();
private:
	//typedef std::shared_ptr<Timer> TimerPtr;
	std::priority_queue<TimerPtr,std::vector<TimerPtr>,TimerCmp> queue_;
	

};
