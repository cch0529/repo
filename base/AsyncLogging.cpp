
#include "AsyncLogging.h"
/*
template<int SIZE>
void FixedBuffer<SIZE>::append(const char* buf,size_t len){
	//printf("in buffer append\n");
	if(avail()>len){
		memcpy(cur_,buf,len);
		cur_+=len;
	}
	//printf("end buffer append\n");
}
*/
AsyncLogging::AsyncLogging(const string& fileName,int flushIntervals):
				currentBuffer_(new Buffer()),
				nextBuffer_(new Buffer()),
				buffers_(),
				mutex_(),
				cond_(mutex_),
				thread_(std::bind(&AsyncLogging::threadFunc,this)),
				running_(false),
				flushIntervals_(flushIntervals),
				logFileName_(fileName){}
void AsyncLogging::append(const char* msg,size_t len){
	//printf("in asyncLogging append\n");
	MutexLockGuard lock(mutex_);
	if(currentBuffer_->avail()>len){
		//printf("ready to append\n");
		currentBuffer_->append(msg,len);
	}
	else{
		buffers_.push_back(std::move(currentBuffer_));
		if(nextBuffer_){
			currentBuffer_=std::move(nextBuffer_);
		}
		else{
			currentBuffer_.reset(new Buffer());
		}
		currentBuffer_->append(msg,len);
		cond_.notify();
	}
}
void AsyncLogging::threadFunc(){
	//printf("in asynclogging threadfunc\n");
	assert(running_);
	BufferPtr newBuffer1(new Buffer());
	BufferPtr newBuffer2(new Buffer());
	//printf("before bzero\n");
	newBuffer1->bzero();
	newBuffer2->bzero();
	//printf("after bzero\n");
	LogFile log(logFileName_,flushIntervals_);
	//log.append("start log\n",11);
	//printf("after create LogFile\n");
	BufferVector buffersToWrite;
	buffersToWrite.reserve(16);
	//printf("ready to run\n");
	while(running_){
		{
			MutexLockGuard lock(mutex_);
			if(buffers_.empty()){
				cond_.waitForSeconds(3);
			}
			//printf("ready to pushback\n");
			buffers_.push_back(std::move(currentBuffer_));
			currentBuffer_=std::move(newBuffer1);
			buffersToWrite.swap(buffers_);
			if(!nextBuffer_){
				nextBuffer_=std::move(newBuffer2);
			}
		}
		assert(!buffersToWrite.empty());
		//printf("buffersToWrite.size:%d\n",buffersToWrite.size());
		if(buffersToWrite.size()>25){
			/*char buf[256];
			snprintf(buf,sizeof buf,"Dropped log messages %zd larger buffers\n",buffersToWrite.size()-2);
			fputs(buf,stderr);
			log.append(buf,strlen(buf));*/
			buffersToWrite.erase(buffersToWrite.begin()+2,buffersToWrite.end());
						
		}
		//printf("ready to write log\n");
		for(const auto& buffer: buffersToWrite){
			//printf("write %uz char to logFile\n",buffer->size());
			log.append(buffer->data(),buffer->size());
		}
		//printf("end write log\n");
		if(buffersToWrite.size()>2){
			buffersToWrite.resize(2);
		}
		if(!newBuffer1){
			assert(!buffersToWrite.empty());
			newBuffer1=std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer1->reset();
		}
		if(!newBuffer2){
			assert(!buffersToWrite.empty());
			newBuffer2=std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer1->reset();
		}
		buffersToWrite.clear();
		log.flush();
		//printf("temply end running\n");
	}
	log.flush();
	
}
