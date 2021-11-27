/*#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>

typedef struct sockaddr SA;*/
#include "network.h"

#define LISTENQUE 5


//void Listen(int fd,int backlog);
//int Wirten(int fd,const void* ptr,int len);
int main(){
	//printf("start\n");
	int listenfd,connfd;
	int backlog=5;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	char buf[BUF_SIZE];
	listenfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//printf("socket\n");
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(SERV_PORT);
	bind(listenfd,(SA*)(&servaddr),sizeof(servaddr));
	//printf("bind\n");
	Listen(listenfd,LISTENQUE);
	//printf("listen\n");
	while(1){
		//printf("inwhile\n");
		socklen_t clilen=sizeof(cliaddr);
		connfd=accept(listenfd,(SA*)(&cliaddr),&clilen);
		if(fork()==0){
			close(listenfd);
			int n;
			while((n=read(connfd,buf,BUF_SIZE))>0){
//				printf("get n words:%d\n",n);
				Writen(connfd,buf,n);
			}
			exit(0);
		}
		close(connfd);
	}
//	printf("outwhile");
	return 0;
		
}
void Listen(int fd,int backlog){
	char *ptr;
//	printf("inlisten\n");
	if((ptr=getenv("LISTENQ"))!=NULL){
		backlog=atoi(ptr);
	}
//	printf("behindgetenv\n");
	listen(fd,backlog);
//	printf("endlisten\n");
}

