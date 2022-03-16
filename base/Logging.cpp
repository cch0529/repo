#include "Logging.h"
static pthread_once_t once_control_=PTHREAD_ONCE_INIT;
static AsyncLogging *asyncLog_;
static string logFileName="./server.log";
void setLogFileName(const string& fileName ){
	logFileName=fileName;
}
void once_init(){
	asyncLog_=new AsyncLogging(logFileName);
	asyncLog_->start();
}
void output(const char* msg,size_t len){
	pthread_once(&once_control_,once_init);
	asyncLog_->append(msg,len);
}

Logger::Logger(const char* fileName,int line):impl_(fileName,line){}

Logger::Impl::Impl(const char* fileName,int line):line_(line),baseName_(fileName){ formatTime();}
Logger::~Logger(){
	impl_.stream_<<"--"<<impl_.baseName_<<':'<<impl_.line_<<'\n';
	output(impl_.stream_.buffer().data(),impl_.stream_.buffer().size());
}
void Logger::Impl::formatTime(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	time_t seconds=tv.tv_sec;
	struct tm* time=localtime(&seconds);
	char str_t[26]={0};
	strftime(str_t,26,"%Y-%m-%d %H:%M:%s\n",time);
	stream_<<str_t;
}
