#include "Util.h"

const int BUFFER_SIZE=4096;
int socket_bind_listen(int port){
	if(port<0||port>65535){
		LOG<<"wrong port";
		return -1;
	}
	//struct sockaddr_in servaddr;
	int listenfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(listenfd<0)
		return -1;
	int optval=1;
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&optval,static_cast<socklen_t>(sizeof optval))==-1){
		LOG<<"failed in setsockopt";
		close(listenfd);
		return -1;
	}
	struct sockaddr_in servaddr;
	bzero(&servaddr,sizeof servaddr);
	servaddr.sin_port=htons(static_cast<uint16_t>(port));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	//LOG<<"listenfd:"<<listenfd;
	if(bind(listenfd,(struct sockaddr*)(&servaddr),(socklen_t)sizeof servaddr)==-1){
		LOG<<"failed in bind";
		LOG<<"errno"<<errno;
		close(listenfd);
		return -1;
	}
	char *ptr=getenv("LISTENQ");
	int backlog=0;
	if(ptr!=NULL){
		backlog=atoi(ptr);
	}
	if(listen(listenfd,backlog)==-1){
		LOG<<"failed in listen";
		close(listenfd);
		return -1;
	}
	LOG<<"listen succed! listenfd:"<<listenfd;
	return listenfd;
}
int setNonblocking(int sockfd){
	int enable=1;
	int flag=fcntl(sockfd,F_GETFL,0);
		if(flag==-1){
			return -1;
		}
		flag|=O_NONBLOCK;
		if(fcntl(sockfd,F_SETFL,flag)==-1){
			return -1;
		}
	
}
int readn(int fd,string& strBuf){
	//char buf[BUFFER_SIZE];
	int nread=0;
	//int rest=size;
	while(true){
		char buf[BUFFER_SIZE];
		int n=read(fd,buf,BUFFER_SIZE);
		if(n==0){
			break;
		}
		else if(n<0){
			if(errno==EINTR)
				continue;
			else if(errno==EAGAIN){
				break;
			}
			else
				return -1;
		}
		else{
			nread+=n;
			//rest-=n;
		}
		strBuf+=string(buf,n);
	}
	//strBuf+=string(buf,nread);
	return nread;
}
int writen(int fd,string& strBuf){
	int nwrite=0;
	const char* ptr=strBuf.c_str();
	int rest=strBuf.size();
	while(rest>0){
		int n=write(fd,ptr,rest);
		if(n<0){
			if(errno==EINTR){
				n=0;
				continue;
			}
			else if(errno==EAGAIN)
				break;
			else
				-1;
		}
		rest-=n;
		nwrite+=n;
		ptr+=n;
	}
	if(nwrite==strBuf.size())
		strBuf.clear();
	else
		strBuf.substr(nwrite);
	return nwrite;

}		
