#pragma once
#include "noncopyable.h"
#include <vector>
#include "Mutex.h"
#include "Condition.h"
#include "LogFile.h"
#include <memory>
#include "Thread.h"
#include <assert.h>
#include <string.h>
const int smallSizeBuffer=4000;
const int largeSizeBuffer=4000*1000;

template<int SIZE>
class FixedBuffer:noncopyable{
public:
	FixedBuffer():cur_(data_){}
	~FixedBuffer(){}
	void append(const char* buf ,size_t len ){
		if(avail()>len){
			memcpy(cur_,buf,len);
			cur_+=len;
		}
	}
	//void append(const char* buf,size_t len);
	const char* data() const { return data_ ;}
	size_t avail(){ return static_cast<size_t>(end()-cur_);}
	size_t size() const { return cur_-data_; }
	void reset(){ cur_=data_;}
	void bzero(){memset(data_,0,SIZE);}
	char* current() { return cur_; }
	void add(size_t len){ cur_+=len; }
private:
	char* end(){ return data_+sizeof(data_);}
	char data_[SIZE];
	char* cur_;
};

class AsyncLogging:noncopyable{

public:
	AsyncLogging(const string& fileName,int flushIntervals=3);
	~AsyncLogging(){
		if(running_){
			stop();	
		}
	}
	void append(const char* msg,size_t len);
	void start(){
		running_=true;
		thread_.start();
	}
	void stop(){
		running_=false;
		thread_.join();
	}
			
private:
	typedef FixedBuffer<largeSizeBuffer> Buffer;
	typedef std::unique_ptr<Buffer> BufferPtr;
	typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
	void threadFunc();
	BufferPtr currentBuffer_;
	BufferPtr nextBuffer_;
	BufferVector buffers_;
	MutexLock mutex_;
	Condition cond_;
	Thread thread_;
	string logFileName_;
	bool running_;	
	const int flushIntervals_;
};
