#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "CurrentThread.h"
#include <unistd.h>
#include <assert.h>
#include "Thread.h"
namespace CurrentThread{
	__thread int t_cachedTid=0;
}
pid_t gettid(){ return static_cast<pid_t>(syscall(SYS_gettid)); }

struct ThreadData{
	Thread::ThreadFunc func_;
	pid_t* tid_;
	CountDownLatch* latch_;
	ThreadData(const Thread::ThreadFunc& func,pid_t* tid,CountDownLatch* latch):func_(func),tid_(tid),latch_(latch){}
										
	void runInThread(){
		*tid_=CurrentThread::tid();
		tid_=NULL;
		latch_->countDown();
		latch_=NULL;
		func_();
	}
};
void CurrentThread::cacheTid(){
	if(t_cachedTid==0){
		t_cachedTid=gettid();
	}	
}
Thread::Thread(ThreadFunc func):func_(func),tid_(0),threadId_(0),started_(false),joined_(false),latch_(1){}
void* startThread(void* obj){
	ThreadData* data=static_cast<ThreadData*>(obj);
	data->runInThread();
	delete data;
	return NULL;
}
void Thread::start(){
	//printf("in thread start\n");
	ThreadData* data=new ThreadData(this->func_,&this->tid_,&this->latch_);
	
	if(pthread_create(&threadId_,NULL,&startThread,data)){
		started_=false;
		delete data;
	}
	else{
		started_=true;
		latch_.wait();
		assert(tid_>0);
	}
	//printf("end thread start\n");
}
int Thread::join(){
	assert(started_);
	assert(!joined_);
	joined_=true;
	return pthread_join(threadId_,NULL);
}
