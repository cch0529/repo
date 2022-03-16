#pragma once
#include "noncopyable.h"
#include <stdio.h>
#include <string>
using std::string;
class AppendFile:noncopyable{
	//using std::string;
public:
	AppendFile(const string&);
	~AppendFile();
	void append(const char* buf,const size_t len);
	void flush();
private:
	size_t write(const char* buf,const size_t len); 
	FILE* file_;
	char buffer[64*1024];
};
