#include "LogFile.h"


LogFile::LogFile(const string& fileName,
		int flushIntervals)
		:file_(fileName),
		flushIntervals_(flushIntervals),
		lastFlush(0){}

void LogFile::flush(){
	file_.flush();
}
void LogFile::append(const char* buf,const size_t len){
	//printf("in Logfile append\n");
	file_.append(buf,len);
	//printf("end apendfile append\n");
	now=time(NULL);
	if(now-lastFlush>=flushIntervals_){
		lastFlush=now;
		file_.flush();
	}
	//printf("end logfile append\n");
}
