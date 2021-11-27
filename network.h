#ifndef _network_h
#define _network_h

#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>

#define SERV_PORT 8888
#define BUF_SIZE 1024

typedef struct sockaddr SA;

void Listen(int fd,int backlog);
int Wirten(int fd,const void* ptr,int len);
int Readn(int fd,void*ptr,int len);
int readline(int fd,void* ptr,int len);

#endif
