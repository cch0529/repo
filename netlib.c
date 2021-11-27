#include "network.h"

int Writen(int fd,const void* buf,int n){
	int nrest=n;
	const char* ptr=buf;
//	printf("inWriten\n");
	while(nrest>0){
//		printf("nrest:%d\n",nrest);
//		printf("inWritenWhile\n");
		int m=write(fd,ptr,nrest);
//		printf("write socket words:%d\n",m);
		if(m>0){
			nrest-=m;
			ptr+=m;
		}
		else
			break;
	}
//	printf("out of Writen While\n");
	return n-nrest;
}
int Readn(int fd,void* buf,int n){
	int nrest=n;
	char *ptr=buf;
	while(nrest){
		int m=read(fd,buf,nrest);
		if(m>0){
			nrest-=m;
			ptr+=m;
		}
		else
			break;
	}
	return n-nrest;
}
int readline(int fd,void* buf,int n){
	char c;
	int count=0;
	char* ptr=buf;
	while(count<n&&read(fd,&c,1)==1){
//		printf("%c\n",c);
		*ptr++=c;
		++count;
		if(c=='\n')
			break;
	}
//	printf("out of readline while\n");
	*ptr=0;
	return count;
}
