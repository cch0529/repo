#pragma once
#include "noncopyable.h"
#include "LogStream.h"
#include <string>
#include <sys/time.h>
#include "AsyncLogging.h"
class Logger{
public:
	Logger(const char*,int);
	~Logger();
	LogStream& stream(){ return impl_.stream_; } 
private:
	class Impl{
	public:
		Impl(const char*,int);
		void formatTime();
		LogStream stream_;
		const std::string baseName_;
		const int line_	;	
	};
	Impl impl_;
	
};
#define LOG Logger(__FILE__,__LINE__).stream()
