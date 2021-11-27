#include "network.h"

int main(int argc,char **argv){
	int sockfd;
	struct sockaddr_in servaddr;
	char recvbuf[BUF_SIZE];
	char sendbuf[BUF_SIZE];
	sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(SERV_PORT);
	inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
	int r=connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
	if(r<0){
		printf("connect error%d\n",r);
		return 1;
	}
	while(1){
//		printf("intwhile\n");
		fgets(sendbuf,BUF_SIZE,stdin);
//		printf("len of sendbuf:%d\n",strlen(sendbuf));
		int m=Writen(sockfd,sendbuf,strlen(sendbuf));
//		printf("write m:%d\n",m);
		//int n=Readn(sockfd,recvbuf,BUF_SIZE);
		//while(
		//int n=read(sockfd,recvbuf,BUF_SIZE);
		//write(1,recvbuf,n);
		//printf(recvbuf);
		//Writen(1,recvbuf,n);
		int n=readline(sockfd,recvbuf,BUF_SIZE);
//		printf("n=%d\n",n);
		fputs(recvbuf,stdout);
		
	}
	return 0;	
}
