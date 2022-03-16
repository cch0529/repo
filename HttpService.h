#pragma once

#include "base/noncopyable.h"
//#include "Channel.h"
#include <memory>
#include "Timer.h"
#include "EventLoop.h"
#include "EventLoop.h"
#include <unistd.h>
#include "Util.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <string>
class Channel;

const int BUFFER_SIZE=4096;
enum ParseState{
	PS_REQUEST,
	PS_HEADERS,
	PS_ANALYSIS,
	PS_FINISHED
};
enum HttpMethod{
	GET,
	POST,
	HEAD,
	UNKNOWN
};
enum ParseRequestState{
	PARSE_REQUEST_ERROR,
	PARSE_REQUEST_SUCCESS,
	PARSE_REQUEST_AGAIN
};
enum ParseHeadersState{
	PARSE_HEADERS_ERROR,
	PARSE_HEADERS_SUCCESS,
	PARSE_HEADERS_AGAIN
};
enum HeadersState{
	H_START
	
};
enum AnalysisState{
	ANALYSIS_SUCCESS,
	ANALYSIS_ERROR_NOTFOUND,
	ANALYSIS_ERROR_METHODNOTALLOWED
};
enum HttpVersion{
	HTTP_10,
	HTTP_11
};
enum ConnectionState{
	CS_CONNECTED,
	CS_DISCONNECTING,
	CS_DISCONNECTED
};
class MimeType{
public:
	static string getMime(const string&);
private:
	static void init();
	static std::unordered_map<string,string> mime;
	MimeType();
	MimeType(const MimeType &);
	static pthread_once_t once_control;
};
class HttpService:noncopyable,
		public std::enable_shared_from_this<HttpService>{
public:
	typedef std::shared_ptr<Channel> ChannelPtr;
	HttpService(EventLoop*,int);
	~HttpService(){ 
		seperateTimer();
		close(sockfd_); 
	//	LOG<<"HTTP SERVICE DISCONSTRUCT"<<'\n';
	}
	
	void setTimer(int timeout){ 
		if(timer_)	
			this->seperateTimer();
		timer_=std::make_shared<Timer>(std::bind(&HttpService::handleClose,this),timeout);
	
	}
	std::shared_ptr<Timer> getTimer(){ return timer_;}
	std::shared_ptr<Channel> getChannel(){ return channel_; }
	void newEvent();	
private:
	void handleClose();
	void handleRead();
	AnalysisState analysisRequest();
	void handleWrite();
	void handleError(int,const string&);
	//void buildResponse();
	void reset();
	ParseRequestState parseRequestLine();
	ParseHeadersState parseHeaders();
	//void parseBody();
	//HttpMethod getMethod(char*);
	//void newEvent();
	void seperateTimer(){
		//close(sockfd_);
		//printf("begin seperateTimer\n");
		timer_->setCanceled();
		timer_.reset();
		//printf("end seperateTimer\n");
	}
	ChannelPtr channel_;
	int sockfd_;
	EventLoop* loop_;
	std::shared_ptr<Timer> timer_;
	//char readBuffer_[BUFFER_SIZE];
	string readBuffer_;
	//char writeBuffer_[BUFFER_SIZE];
	string writeBuffer_;
	const char* readPos_;
	//int numRead_;
	//int numWrite_;
	std::unordered_map<string,string> headers_;
	HttpMethod method_;
	const string defaultFileName_;
	string filename_;
	HttpVersion version_;
	ParseState state_;
	bool error_;	
	ConnectionState connectionState_;
	bool keepAlive_;
};

