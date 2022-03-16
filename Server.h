#pragma once
#include "base/noncopyable.h"
#include "EventLoop.h"
#include <memory>
#include "Channel.h"
#include "EventLoopThreadPool.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "Util.h"
#include <netinet/tcp.h>
class Server:noncopyable{
public:
	Server(EventLoop*,int,int);
	~Server(){}
	void start();
private:
	void onNewConnection();
	EventLoop* loop_;
	std::shared_ptr<Channel> acceptChannel_;
	int port_;
	int listenfd_;
	bool started_ ;
	std::unique_ptr<EventLoopThreadPool> threadPool_;	
};
