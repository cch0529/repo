#include"../base/CurrentThread.h"
#include "../EventLoop.h"
#include<pthread.h>
#include <unistd.h>
#include <stdio.h>
void *threadFunc(void *){
	printf("threadFunc:pid=%d,tid=%d",getpid(),CurrentThread::tid());
	EventLoop loop;
	loop.loop();
	return ((void*)0);
}

int main(){
	printf("main:pid=%d,tid=%d",getpid(),CurrentThread::tid());
	EventLoop loop;
	
thread_t tid_t;
	pthread_create(&tid_t,NULL,threadFunc,NULL);
	loop.loop();
	
}
