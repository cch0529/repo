#include "Server.h"

Server::Server(EventLoop* loop,int port,int threadNum):loop_(loop),
				acceptChannel_(new Channel(loop_)),
				port_(port),
				started_(false),
				threadPool_(new EventLoopThreadPool(loop,threadNum)){
	listenfd_=socket_bind_listen(port);
	acceptChannel_->setFd(listenfd_);
	if(setNonblocking(listenfd_)==-1){
		//perror("listenfd setNonblocking failed");
		//abort();
	}	
	
}
void Server::onNewConnection(){
	struct sockaddr_in clientaddr;
	bzero(&clientaddr,sizeof(clientaddr));
	socklen_t client_addr_len=sizeof clientaddr;
	int sockfd=0;
	//LOG<<"on newConnection";
	while((sockfd=accept(listenfd_,(struct sockaddr*)(&clientaddr),&client_addr_len))>0){
		//LOG<<"New connection from"<<inet_ntoa(clientaddr.sin_addr)<<":"<<ntohs(clientaddr.sin_port);
		int enable=1;
		//LOG<<"sockfd:"<<sockfd;
		if(setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,&enable,static_cast<socklen_t>(sizeof enable))==-1){
			LOG<<"failed in setnodelay";
			LOG<<"errno:"<<errno;
			continue;
		}	
		if(setNonblocking(sockfd)==-1){
			LOG<<"failed in setnonblocking";
			continue;
		}
		EventLoop* nextLoop=threadPool_->getNextLoop();
		std::shared_ptr<HttpService> httpservice(new HttpService(nextLoop,sockfd));
		std::shared_ptr<Channel> chan=httpservice->getChannel();
		chan->setHolder(httpservice);
		nextLoop->queueInLoop(std::bind(&HttpService::newEvent,httpservice));
		//chan->setEvents(EPOLLIN | EPOLLET | EPOLLONESHOT);
		//EventLoop* curLoop=threadPool_->getNextLoop();
		//curLoop->addToPoller(chan);
		//printf("add sockfd:%d succed",sockfd);
	}	
	acceptChannel_->setEvents(EPOLLIN|EPOLLET);

}
void Server::start(){
	assert(!started_);
	threadPool_->start();
	acceptChannel_->setEvents(EPOLLIN | EPOLLET);
	acceptChannel_->setReadCallBack(std::bind(&Server::onNewConnection,this));
	loop_->addToPoller(acceptChannel_);
	started_=true;
}

