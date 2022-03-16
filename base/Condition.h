#pragma once
#include "Mutex.h"

class Condition:noncopyable{
public:
	explicit Condition(MutexLock& mutex):mutex_(mutex){ pthread_cond_init(&cond,NULL);}
	~Condition(){ pthread_cond_destroy(&cond);}
	void wait(){ pthread_cond_wait(&cond,mutex_.getMutex());}
	void notify(){ pthread_cond_signal(&cond);}
	void notifyAll(){ pthread_cond_broadcast(&cond);}
	bool waitForSeconds(int );
private:
	MutexLock& mutex_;
	pthread_cond_t cond;
};
