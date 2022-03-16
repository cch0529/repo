#include"../base/CurrentThread.h"
#include "../EventLoop.h"
#include<pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include "../base/Mutex.h"
#include "../base/FileUtil.h"
#include "../base/LogFile.h"
#include "../base/Condition.h"
#include "../base/CountDownLatch.h"
#include "../base/Thread.h"
#include "../base/AsyncLogging.h"
#include "../base/LogStream.h"
#include "../base/Logging.h"
#include "../Channel.h"
#include "../Timer.h"
#include <memory>
#include "../Epoller.h"
#include <fcntl.h>
#include <sys/epoll.h>
#include "../EventLoopThread.h"
#include "../EventLoopThreadPool.h"
#include "../Server.h"

void setLogFileName(const string&);
int main(int argc,char* argv[]){
	int threadNum=5;
	int port=80;
	string logFileName_="./server.log";
	int opt;
	const char* str="t:l:p:";
	while((opt=getopt(argc,argv,str))!=-1){
		switch(opt){
			case 't':
				threadNum=atoi(optarg);
				break;
			case 'l':
				logFileName_=optarg;
				break;
			case 'p':
				port=atoi(optarg);
				break;
			default:
				break;
		}	
	}	
	setLogFileName(logFileName_);


	//printf("in main\n");
	EventLoop loop;
	Server server(&loop,port,threadNum);
	server.start();
	loop.loop();
	//while(1){}
}




