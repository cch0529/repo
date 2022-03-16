#include "FileUtil.h"
#include <string>

AppendFile::AppendFile(const string& fileName):file_(fopen(fileName.c_str(),"ae")){
	setbuffer(file_,buffer,sizeof(buffer));
}
AppendFile::~AppendFile(){
	fclose(file_);
}
void AppendFile::append(const char* buf,const size_t len){
	size_t n=len;
	//printf("in appendfile::append,len=%zu\n",n);
	while(n!=0){
		//printf("ready to appendfile::write\n");
		size_t nwrite=this->write(buf,n);
		//printf("nwrite:%zu\n",nwrite);
		if(nwrite==0){
			int err=ferror(file_);
			if(err)
				fprintf(stderr,"AppendFile::append() failed!\n");
			break;
		}
		n-=nwrite;
	}
	//printf("end appendfile::apend\n");
}
size_t AppendFile::write(const char* buf,const size_t len){
	//printf("in appendfile write\n");
	return fwrite_unlocked(buf,1,len,file_);
	//printf("end appendfile write\n");
}
void AppendFile::flush(){
	//this->append("flush\n",7);
	fflush(file_);	
}
