#pragma once
#include "AsyncLogging.h"
#include "noncopyable.h"
#include <string>
#include <algorithm>
class LogStream:noncopyable{
public:	
	typedef FixedBuffer<smallSizeBuffer> Buffer;
	LogStream& operator<<(const char*);
	LogStream& operator<<(const string&);
	LogStream& operator<<(const unsigned char*);
	LogStream& operator<<(bool);
	LogStream& operator<<(char);
	LogStream& operator<<(short);
	LogStream& operator<<(unsigned short);
	LogStream& operator<<(int);
	LogStream& operator<<(unsigned int);
	LogStream& operator<<(long);
	LogStream& operator<<(unsigned long);
	LogStream& operator<<(long long);
	LogStream& operator<<(unsigned long long);
	LogStream& operator<<(float);
	LogStream& operator<<(double);
	LogStream& operator<<(long double);
	const Buffer& buffer() const { return buffer_;}
	void resetBuffer(){ buffer_.reset(); }
	void append(const char* msg,size_t len){ buffer_.append(msg,len); }
private:
	//typedef FixedBuffer<smallSizeBuffer> Buffer;
	template<typename T>
	void formatInteger(T);	
	Buffer buffer_;
	static const int kMaxNumericSize=32;	
};
