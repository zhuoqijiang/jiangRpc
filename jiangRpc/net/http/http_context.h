#pragma once

#include <string>
#include <map>
#include <memory>
#include "http_enum.h"

namespace jiangRpc {

class HttpHandler;
class HttpServer;
class HttpContext {
public:
	friend class HttpHandler;
	HttpContext(HttpHandler* handler);
	HttpMethod method()
	{
		return method_;
	}
	std::string path() 
	{
		return path_;
	}
	HttpVersion version()
	{
		return version_;
	}
	std::string body()
	{
		return body_;
	}
	
	std::string clientIp()
	{
		return clientIp_;
	}

	std::string localIp()
	{
		return localIp_;
	}

	void addHeader(const std::string& key, const std::string& value)
	{
		respHeaders_.insert(std::make_pair(key, value));
	}
	const std::map<std::string, std::string>& querys()
	{
		return querys_;
	}


	void string(HttpStatus httpStatus,const std::string& str);
	void html(HttpStatus httpStatus, std::string filePath);
	void form(HttpStatus httpStatus, std::map<std::string,std::string> m);
private:
	void reset();

	std::string mapToHeader();
private:
	HttpMethod method_ = HttpMethod::HTTP_GET;
	std::string path_;
	std::map<std::string, std::string> querys_;	
	HttpVersion version_ = HTTP_11;
	std::map<std::string, std::string> headers_;
	std::string body_;
	
	std::string clientIp_;
	std::string localIp_;
	std::map<std::string, std::string> respHeaders_;

	HeaderState headerState_ = H_KEY;
	ProcessState processState_ = PARSE_LINE;
	HttpHandler* handler_;
};

}
