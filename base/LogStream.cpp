#include "LogStream.h"
const char  digits[]="9876543210123456789";
const char* zero=digits+9;

template class FixedBuffer<smallSizeBuffer>;
template class FixedBuffer<largeSizeBuffer>;

template<typename T>
size_t convert(char* buf,T v){
	char* p=buf;
	T i=v;
	while(i){
		int lsd=static_cast<int>(i%10);
		*p++=zero[lsd];
		i/=10;
	}
	if(v<0)
		*p++='-';
	*p='\0';
	std::reverse(buf,p);
	return p-buf;
}

LogStream& LogStream::operator<<(const char* str){
	if(str)
		buffer_.append(str,strlen(str));
	else
		buffer_.append("(null)",6);
	return *this;
}
LogStream& LogStream::operator<<(const unsigned char* str){
	return operator<<(reinterpret_cast<const char*>(str));
}
LogStream& LogStream::operator<<(const string& str){
	buffer_.append(str.c_str(),str.size());
	return *this;
}
LogStream& LogStream::operator<<(bool v){
	buffer_.append(v?"true":"false",v?4:5);
	return *this;
}
LogStream& LogStream::operator<<(char c){
	buffer_.append(&c,1);
	return *this;
}
template<typename T>
void LogStream::formatInteger(T v){
	if(buffer_.avail()>=kMaxNumericSize){
		size_t len=convert(buffer_.current(),v);
		buffer_.add(len);
	}	
}
LogStream& LogStream::operator<<(short v){
	*this<<static_cast<int>(v);
	return *this;
}
LogStream& LogStream::operator<<(unsigned short v){
	*this<<static_cast<unsigned int>(v);
	return *this;
}
LogStream& LogStream::operator<<(int v){
	formatInteger(v);
	return *this;
}
LogStream& LogStream::operator<<(unsigned int v){
	formatInteger(v);
	return *this;
}
LogStream& LogStream::operator<<(long v){
	formatInteger(v);
	return *this;
}
LogStream& LogStream::operator<<(unsigned long v){
	formatInteger(v);
	return *this;
}
LogStream& LogStream::operator<<(long long v){
	formatInteger(v);
	return *this;
}
LogStream& LogStream::operator<<(unsigned long long v){
	formatInteger(v);
	return *this;
}
LogStream& LogStream::operator<<(float v){
	*this<<static_cast<double>(v);
	return *this;
}
LogStream& LogStream::operator<<(double v){
	if(buffer_.avail()>=kMaxNumericSize){
		size_t len=snprintf(buffer_.current(),kMaxNumericSize,"%.12g",v);
		buffer_.add(len);	
	}
	return *this;
}
LogStream& LogStream::operator<<(long double v){
	if(buffer_.avail()>=kMaxNumericSize){
		size_t len=snprintf(buffer_.current(),kMaxNumericSize,"%.12Lg",v);
		buffer_.add(len);	
	}
	return *this;

}
