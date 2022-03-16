#pragma once
#include "noncopyable.h"
#include "FileUtil.h"
class LogFile:noncopyable{
public:
	LogFile(const string& fileName,int flushIntervals=3);
	//~LogFile();
	void append(const char*,const size_t);
	void flush();
private:
	//void append_unlocked();
	time_t now;
	time_t lastFlush;
	const int flushIntervals_;
	AppendFile file_;	
};
