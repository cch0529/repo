#pragma once
#include <pthread.h>
#include "noncopyable.h"

class MutexLock:noncopyable{
public:
	MutexLock(){
		pthread_mutex_init(&mutex,NULL);
	}
	~MutexLock(){
		pthread_mutex_destroy(&mutex);
	}
	void lock(){
		pthread_mutex_lock(&mutex);
	}
	void unlock(){
		pthread_mutex_unlock(&mutex);
	}
	pthread_mutex_t* getMutex(){ return &mutex;}
private:
	pthread_mutex_t mutex;
};

class MutexLockGuard:noncopyable{
public:
	explicit MutexLockGuard(MutexLock &mutex):mutex_(mutex){
		mutex_.lock();
	}
	~MutexLockGuard(){
		mutex_.unlock();
	}
private:
	MutexLock &mutex_;
};
