
#include "http_server.h"
#include "http_handler.h"
#include "../socket.h"
#include <string>

const std::string serverName = "ZhuoqiJiang's Database Server";

HttpContext::HttpContext(HttpHandler* handler): handler_(handler){
	localIp_ = handler_->conn_->getLocalAddress().addr;
	clientIp_ = handler_->conn_->getRemoteAddress().addr;
}


void HttpContext::string(HttpStatus httpStatus, const std::string& str)
{
	addHeader("Content-Type", g_HttpContentTypeMap.at(HTTP_TEXT_PLAIN));
	addHeader("Content-Length", std::to_string(str.size()));
	addHeader("Server", serverName);
	std::string header, response;
	header += g_HttpVersionMap.at(version_) + " " + g_HttpStatusMap.at(httpStatus) + "\r\n";
	//header += "Content-Type: " + g_HttpContentTypeMap.at(HTTP_TEXT_PLAIN) + "\r\n";
	//header += "Content-Length: " + std::to_string(str.size()) + "\r\n";
	//header += "Server: " +  serverName + "\r\n";
	
	

	auto conIter = headers_.find("Connection");
	if (conIter != headers_.end()) {
		addHeader("Connection", conIter->second);
		//header += "Connection: " + conIter->second + "\r\n"; 
	}
	header += mapToHeader();
	response = header + "\r\n";

	if (method_ != HttpMethod::HTTP_HEAD) {
		response += str;
	}
	
	handler_->conn_->write(response); 
}

void HttpContext::reset()
{ 
	path_.clear();
	querys_.clear();
	headers_.clear();
	body_.clear();
	
	headerState_ = H_KEY;
	processState_ = PARSE_LINE;
}


std::string HttpContext::mapToHeader()
{
	std::string header;
	for (const auto& h: respHeaders_) {
		header += h.first + ": " + h.second + "\r\n";
	}
	
	return header;
}
