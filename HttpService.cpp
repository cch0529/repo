#include "HttpService.h"
#include "Channel.h"
pthread_once_t MimeType::once_control =PTHREAD_ONCE_INIT;
std::unordered_map<string,string> MimeType::mime;
const int DEFAULT_KEEP_ALIVE_TIME=5*60*1000;
const int DEFAULT_EXPIRED_TIME=2000;
void MimeType::init(){
	mime[".html"]="text/html";
	mime[".avi"]="video/x-msvideo";
	mime[".bmp"]="image/bmp";
	mime[".c"]="text/plain";
	mime[".doc"]="application/msword";
	mime[".gif"]="image/gif";
	mime[".gz"]="application/x-gzip";
	mime[".htm"]="text/html";
	mime[".ico"]="image/x-icon";
	mime[".jpg"]="image/jpeg";
	mime[".png"]="image/png";
	mime[".txt"]="text/plain";
	mime[".mp3"]="audio/mp3";
	mime["default"]="text/html";
}
string MimeType::getMime(const string& suffix){
	pthread_once(&once_control,MimeType::init);
	if(mime.find(suffix)==mime.end())
		return mime["default"];
	else
		return mime[suffix];
}
HttpService::HttpService(EventLoop* loop,int sockfd):loop_(loop),sockfd_(sockfd),
							defaultFileName_("index.html"),
							error_(false),
							keepAlive_(false),
							state_(PS_REQUEST),
							connectionState_(CS_CONNECTED),
							channel_(new Channel(loop,sockfd)){
	channel_->setReadCallBack(std::bind(&HttpService::handleRead,this));
	channel_->setWriteCallBack(std::bind(&HttpService::handleWrite,this));
}
void HttpService::reset(){
	filename_.clear();
	headers_.clear();
	state_=PS_REQUEST;	
}
void HttpService::handleClose(){
	std::shared_ptr<HttpService> guard(shared_from_this());
	loop_->removeFromPoller(getChannel());
	//LOG<<"httpservice closed,fd:"<<sockfd_<<"\n";	
}
void HttpService::handleRead(){
	//LOG<<"call httpservice handleREAD";
	if(connectionState_==CS_DISCONNECTING)
		return;
	//handleError(404,"Not Found");
	int nread=readn(sockfd_,readBuffer_);
	if(nread<0){
		error_=true;
		handleError(500,"Internal Server Error");
		connectionState_=CS_DISCONNECTING;
		LOG<<"readn wrong\n";			
	}
	else if(nread==0){
		connectionState_=CS_DISCONNECTING;
	}
	LOG<<readBuffer_;
	while(readBuffer_.size()>0){
		/*if(nread<0){
			error_=true;
			handleError(500,"Internal Server Error");
			connectionState_=CS_DISCONNECTING;
			LOG<<"readn wrong\n";
			break;
		}
		else if(nread==0){
			connectionState_=CS_DISCONNECTING;
			break;
		}
		LOG<<readBuffer_;*/
		if(state_==PS_REQUEST){
			ParseRequestState r_st=parseRequestLine();
			if(r_st==PARSE_REQUEST_AGAIN)
				break;
			else if(r_st==PARSE_REQUEST_SUCCESS){
				state_=PS_HEADERS;
			}
			else{
				error_=true;
				connectionState_=CS_DISCONNECTING;
				handleError(400,"Bad Request");
				break;
			}
		}
		if(state_==PS_HEADERS){
			ParseHeadersState h_st=parseHeaders();
			if(h_st==PARSE_HEADERS_AGAIN)
				break;
			else if(h_st==PARSE_HEADERS_SUCCESS){
					state_=PS_ANALYSIS;
				}
			else{
				error_=true;
				connectionState_=CS_DISCONNECTING;
				handleError(400,"Bad Request");
				break;
			}
		}
		if(state_==PS_ANALYSIS){
			AnalysisState a_st=analysisRequest();
			if(a_st==ANALYSIS_ERROR_NOTFOUND){
				error_=true;
				handleError(404,"Not Found!");
				break;
			}
			else if(a_st==ANALYSIS_ERROR_METHODNOTALLOWED){
				error_=true;
				handleError(405,"Method Not Allowed");
				break;
			}
			else if(a_st==ANALYSIS_SUCCESS){
				this->reset();
				//state_=PS_REQUEST;
			}
				
		}
	}
	//for(auto itr:headers_)
	//	LOG<<itr.first<<":"<<itr.second<<'\n';
	//buildResponse();
	if(connectionState_==CS_CONNECTED){
		uint32_t events=channel_->getEvents();
		if(keepAlive_){
			this->setTimer(DEFAULT_KEEP_ALIVE_TIME);
			//LOG<<"KEEP ALIVE\n";
			//events|=EPOLLIN|EPOLLET;
		}	
		else{
			//LOG<<"no keep alive\n";
			this->setTimer(DEFAULT_EXPIRED_TIME);
		}
		events|=EPOLLIN|EPOLLET;
		channel_->setEvents(events);
	}
	LOG<<writeBuffer_<<'\n';
	//printf("begin to handle write\n");
	handleWrite();
	//printf("end handleRead");
}
void HttpService::newEvent(){
	channel_->setEvents(EPOLLIN | EPOLLET | EPOLLONESHOT);
	loop_->addToPoller(channel_,DEFAULT_EXPIRED_TIME);	

}
ParseRequestState HttpService::parseRequestLine(){
	//const char* end=std::find(begin,totalEnd,'\r');
	size_t enter=readBuffer_.find("\r\n");
	if(enter==string::npos)
		return PARSE_REQUEST_AGAIN;
	string requestLine=readBuffer_.substr(0,enter);
	if(readBuffer_.size()>enter+2)
		readBuffer_=readBuffer_.substr(enter+2);
	else
		readBuffer_.clear();
	size_t curPos;
	if((curPos=requestLine.find("GET"))!=string::npos)
		method_=GET;
	else if((curPos=requestLine.find("POST"))!=string::npos)
		method_=POST;
	else if((curPos=requestLine.find("HEAD"))!=string::npos)
		return PARSE_REQUEST_ERROR;
	size_t backSlash=requestLine.find('/',curPos);
	if(backSlash==string::npos){
		filename_="index.html";
		version_=HTTP_11;
		return PARSE_REQUEST_SUCCESS;
	}
	else{
		size_t space=requestLine.find(' ',backSlash);
		if(space==string::npos)
			return PARSE_REQUEST_ERROR;
		if(space-backSlash>1){
			filename_=requestLine.substr(backSlash+1,space-backSlash-1);
			size_t questionMark=filename_.find('?');
			if(questionMark!=string::npos)
				filename_=filename_.substr(0,questionMark);
		}
		else
			filename_="index.html";
		curPos=space;
	}
	string versionCode=requestLine.substr(requestLine.size()-3,3);
	if(versionCode=="1.0")
		version_=HTTP_10;
	else if(versionCode=="1.1")
		version_=HTTP_11;
	else 
		return PARSE_REQUEST_ERROR;
	
	return PARSE_REQUEST_SUCCESS;

}
ParseHeadersState HttpService::parseHeaders(){
	//readPos_=begin;
	int curPos=0;
	int cutDownPos=0;
	int n=readBuffer_.size();
	size_t kStart,kEnd,vStart,vEnd;
	while(curPos<n){
		if(readBuffer_[curPos]=='\r'){
			if(n>curPos+2){
				readBuffer_=readBuffer_.substr(curPos+2);
			}
			else if(n==curPos+2){
				readBuffer_.clear();
			}
			else{
				//readBuffer_=readBuffer_.substr(cutDownPos);
				break;
			}
			return PARSE_HEADERS_SUCCESS;
		}
		kStart=curPos;
		size_t colon=readBuffer_.find(':',curPos);
		if(colon==string::npos)
			break;
		kEnd=colon;
		vStart=curPos=colon+2;
		size_t enter=readBuffer_.find('\r',curPos);
		if(enter==string::npos)
			break;
		vEnd=enter;
		if(kEnd-kStart<=0 || vEnd-vStart<=0)
			return PARSE_HEADERS_ERROR;
		headers_[readBuffer_.substr(kStart,kEnd-kStart)]=readBuffer_.substr(vStart,vEnd-vStart);
		cutDownPos=vEnd;
		curPos=vEnd+2;
	}
	readBuffer_.substr(cutDownPos);
	return PARSE_HEADERS_AGAIN;
}
//void HttpService::parseBody(){	
//	LOG<<"int parseBody\n";

//}

void HttpService::handleWrite(){
	//printf("in handle write\n");
	uint32_t events=channel_->getEvents();
	//printf("writeBuffersize:%d\n",writeBuffer_.size());
	int n=writen(sockfd_,writeBuffer_);
	//printf("n:%d\n",n);
	if(n<0){
		connectionState_=CS_DISCONNECTING;
	}
		
	if(writeBuffer_.size()>0){
		events|=EPOLLOUT | EPOLLET;			
		channel_->setEvents(events);
	}
	if(connectionState_==CS_DISCONNECTING){	
		//printf("cope disconnecting\n");		
		//seperateTimer();
		//LOG<<"ready to close\r\n";
		loop_->runInLoop(std::bind(&HttpService::handleClose,shared_from_this()));
		//printf("end cope disconnecting\n");
	}
	//printf("end handle write\n");
	//write(sockfd_,writeBuffer_.c_str(),writeBuffer_.size());
}
/*
void HttpService::buildResponse(){
	string output;
	output+="HTTP/1.1 200 OK\r\n";
	output+="Server: cch's server\r\n";
	output+="Content-Type: text/html\r\n";
	struct stat sbuf;
	stat(filename_.c_str(),&sbuf);
	output+="Content-Length: "+std::to_string(sbuf.st_size)+"\r\n";
	output+="\r\n";
	int fd=open(filename_.c_str(),O_RDONLY,0);
	void *mmapRet=mmap(NULL,sbuf.st_size,PROT_READ,MAP_PRIVATE,fd,0);
	close(fd);
	char *char_addr=static_cast<char *>(mmapRet);
	output+=string(char_addr,char_addr+sbuf.st_size);
	munmap(mmapRet,sbuf.st_size);
	numWrite_=output.size();
	writeBuffer_+=output;
	//memcpy(writeBuffer_,output.c_str(),numWrite_);
	
}*/
void HttpService::handleError(int errNo,const string& errMsg){
	string bodyBuf,headerBuf;
	bodyBuf+="<html><title>something wrong happened</title>";
	bodyBuf+="<body bgcolor=\"ffffff\">";
	bodyBuf+=std::to_string(errNo)+errMsg;
	bodyBuf+="<hr><em> CCH's web server</em>\n</body></html>";

	headerBuf+="HTTP/1.1 "+std::to_string(errNo)+errMsg+"\r\n";
	headerBuf+="Content-Type: text/html\r\n";
	headerBuf+="Connection: Close\r\n";
	headerBuf+="Content-Length: "+std::to_string(bodyBuf.size())+"\r\n";
	headerBuf+="Server: cch's web server\r\n";
	headerBuf+="\r\n";
	
	writeBuffer_+=headerBuf+bodyBuf;
	//handleWrite();	
}
AnalysisState HttpService::analysisRequest(){
	if(method_==POST);
	else if(method_==GET || method_==HEAD){
		string header;
		header+="HTTP/1.1 200 OK\r\n";
		if(headers_.find("Connection")!=headers_.end()&&
			(headers_["Connection"]=="keep-alive"||headers_["Connection"]=="Keep-Alive"||headers_["Connection"]=="Keep-alive")){
			keepAlive_=true;
			header+=string("Connection: Keep-Alive\r\n")+"Keep-Alive: timeout="+std::to_string(DEFAULT_KEEP_ALIVE_TIME)+"\r\n"; 
		}
		size_t dotPos=filename_.find('.');
		string fileType;
		if(dotPos==string::npos){
			fileType=MimeType::getMime("default");
		}
		else{
			fileType=MimeType::getMime(filename_.substr(dotPos));
		}
		struct stat sbuf;
		if(stat(filename_.c_str(),&sbuf)<0){
			return ANALYSIS_ERROR_NOTFOUND;
		}
		header+="Content-Type: "+fileType+"\r\n";
		header+="Content-Length: " + std::to_string(sbuf.st_size)+"\r\n";
		header+="Server: cch's web server\r\n";
		header+="\r\n";
		if(method_==HEAD){
			writeBuffer_+=header;
			return ANALYSIS_SUCCESS;
		}
		int fd=open(filename_.c_str(),O_RDONLY,0);
		if(fd<0){
			return ANALYSIS_ERROR_NOTFOUND;
		}
		void *mmapRet = mmap(NULL,sbuf.st_size,PROT_READ,MAP_PRIVATE,fd,0);
		close(fd);
		if(mmapRet==(void*)-1){
			munmap(mmapRet,sbuf.st_size);
			return ANALYSIS_ERROR_NOTFOUND;
		}
		char * srcAddr=reinterpret_cast<char *>(mmapRet);
		writeBuffer_+=header+string(srcAddr,srcAddr+sbuf.st_size);
		munmap(mmapRet,sbuf.st_size);
		return ANALYSIS_SUCCESS;
	}
	return ANALYSIS_ERROR_METHODNOTALLOWED;
}
