#include <memory>
#include <utility>
#include "http_handler.h"
#include "http_server.h"
#include "../socket.h"
HttpHandler::HttpHandler(HttpServer* server, Conn* conn)
:server_(server), conn_(conn){}

HttpHandler::~HttpHandler()
{
}

void HttpHandler::handle()
{
	
	HttpContext httpContext(this);
	ParseState state;

	while (true) {
		std::string buf;
		int n = 0;
		if ((n = conn_->read(buf)) == 0) {
			break;
		}


		buffer_.append(buf.substr(0, n));

		state = parse(&httpContext);
		
		if (nowReadPos_ == static_cast<int>(buffer_.size())) {
			nowReadPos_ = 0;
			buffer_.clear();
		}

		if (state == ParseState::AGAIN) 
			continue;
		
		if (state == ParseState::ERROR) {
			(server_->getError())(httpContext);
			break;
		}
		
		if (state == ParseState::SUCCESS) {
			if (!analyse(&httpContext))
				break;
			httpContext.reset();
			state = AGAIN;
		}
	}
	conn_->close();
}


ParseState HttpHandler::parse(HttpContext* httpContext)
{
	ParseState state;

	switch(httpContext->processState_) 
	{
	case ProcessState::PARSE_LINE:
		state = parseLine(httpContext);
		if (state != ParseState::SUCCESS)
			return state;
		httpContext->processState_ = ProcessState::PARSE_HEADER;

	case ProcessState::PARSE_HEADER:
		state = parseHeaders(httpContext);
		if (state != ParseState::SUCCESS)
			return state;
		httpContext->processState_ = ProcessState::PARSE_BODY;

	case ProcessState::PARSE_BODY:
		state = parseBody(httpContext);
		if (state != ParseState::SUCCESS)
			return state;
		httpContext->processState_ = ProcessState::PARSE_SUCCESS;

	case ProcessState::PARSE_SUCCESS:
		return ParseState::SUCCESS;
	}
	return ParseState::ERROR; 
}

ParseState HttpHandler::parseLine(HttpContext* httpContext)
{
	std::string &str = buffer_;
	size_t pos = str.find("\r\n", nowReadPos_);
	if (pos == str.npos)
		return ParseState::AGAIN;
	std::string line = str.substr(nowReadPos_, pos - nowReadPos_);
	nowReadPos_ = pos + 2;
	size_t methodPos = 0, uriPos = 0, queryPos = 0, versionPos = 0, spacePos = 0;

	// method
	spacePos = line.find(' ', methodPos);
	if (spacePos == line.npos)
		return ParseState::ERROR;
	std::string method = line.substr(methodPos, spacePos - methodPos);
	if (g_HttpMethodMap.find(method) == g_HttpMethodMap.end())
		return ParseState::ERROR;
	httpContext->method_ = g_HttpMethodMap.at(method);
	uriPos = spacePos + 1;

	//uri
	spacePos = line.find(' ', uriPos);
	if (spacePos == line.npos) 
		return ParseState::ERROR;
	std::string uri = line.substr(uriPos, spacePos - uriPos);
	queryPos = uri.find('?');
	if (queryPos  == line.npos) {
		httpContext->path_ = uri;
	}
	else {
		httpContext->path_ = uri.substr(0, queryPos);
		if (!parseQuery(httpContext, uri.substr(queryPos + 1))) {
			return ParseState::ERROR;
		}
	}
	versionPos = spacePos + 1;
	//version
	
	std::string version = line.substr(versionPos);
	if (version == "HTTP/1.0") 
		httpContext->version_ = HttpVersion::HTTP_10;
	else if (version == "HTTP/1.1")
		httpContext->version_ = HttpVersion::HTTP_11;
	else
		return ParseState::ERROR;

	return ParseState::SUCCESS;
}

bool HttpHandler::parseQuery(HttpContext* httpContext, const std::string& query)
{
	int flag = 0;
	//a = 2 & b = 3
	//0   1  0   1
	std::string key, value;
	int querySize = query.size();
	for (int i = 0; i < querySize; i++) {
		switch (flag)
		{
		case 0:
			if (query[i] == '=') {
				flag = 1;
				break;
			}
			if (query[i] != '&') {
				key += query[i];
				break;
			}
			return false;
	   case 1:
			if (query[i] != '=' && query[i] != '&') {
				value += query[i];
			}
			if (i + 1 == querySize || query[i] == '&') {
				if (key == "" || value == "") 
					return false;
				auto pair = httpContext->querys_.insert(std::make_pair(key, value));
				if (!pair.second)
					return false;
				key.clear();
				value.clear();
				flag = 0;
			}
			break;
		}
	}
	return true;
}

bool HttpHandler::checkKeyName(const std::string key)
{
	if (!(isalpha(key[0]) || key[0] == '_'))
		return false;

	int keySize = key.size();
	for (int i = 1; i < keySize; i++) {
		if (!(isalnum(key[i]) || key[i] == '_')) 
			return false;
	}
	return true;
}

ParseState HttpHandler::parseHeaders(HttpContext* httpContext)
{
	std::string &str = buffer_;
	size_t pos = str.find("\r\n\r\n", nowReadPos_);
	if (pos == str.npos)
		return ParseState::AGAIN;

	std::string key, value;
  	bool notFinish = true;
	int strSize = str.size();
  	for (; nowReadPos_ < strSize && notFinish; nowReadPos_++) {
		switch (httpContext->headerState_) {

      	case H_KEY:
        	if (str[nowReadPos_] == ':') {
          		httpContext->headerState_ = H_COLON;
				break;
        	}
			if (str[nowReadPos_] == '\r' || str[nowReadPos_] == '\n') {
				return ParseState::ERROR;
			}
			key += str[nowReadPos_];
			break;
        	
      	case H_COLON:
        	if (str[nowReadPos_] == ' ') {
          		httpContext->headerState_ = H_VALUE;
				break;
        	}
          	return ParseState::ERROR;

      	case H_VALUE: 
        	if (str[nowReadPos_] == '\r') {
         			httpContext->headerState_ = H_CR;
        			break;
        	}
			value += str[nowReadPos_];
			break;
		       	
      	case H_CR:
        	if (str[nowReadPos_] == '\n') {
          		httpContext->headerState_ = H_LF;
          		auto pair = httpContext->headers_.insert(std::make_pair(key, value));
				if (!pair.second)
					return ParseState::ERROR;
				key.clear();
				value.clear();
				break;
        	}
			return ParseState::ERROR;

      	case H_LF:
       		if (str[nowReadPos_] == '\r') {
          		httpContext->headerState_ = H_END_CR;
				break;
        	}
			nowReadPos_--;
 		    httpContext->headerState_ = H_KEY;
       		break;

      	case H_END_CR:
        	if (str[nowReadPos_] == '\n') {
				httpContext->headerState_ = H_END_LF;
				notFinish = false;
				break;
       		}
          	return ParseState::ERROR;
		case H_END_LF:
			return ParseState::ERROR;

		}
	}
  	return ParseState::SUCCESS;
}

ParseState HttpHandler::parseBody(HttpContext* httpContext)
{
	auto it = httpContext->headers_.find("Content-Length");
	if (it == httpContext->headers_.end()) {
		return ParseState::SUCCESS;
	}
	int length = std::stoi(it->second);
	if (static_cast<int>(buffer_.size()) - nowReadPos_ < length) {
		return ParseState::AGAIN;
	}
	httpContext->body_ = buffer_.substr(nowReadPos_, length);
	nowReadPos_ += length;
	return ParseState::SUCCESS;
}

bool HttpHandler::analyse(HttpContext* httpContext)
{
	switch (httpContext->method_)
	{

	case HTTP_GET:
		(server_->searchGET(httpContext->path_))(*httpContext);
		break;
	case HTTP_POST:
		(server_->searchPOST(httpContext->path_))(*httpContext);
		break;
	case HTTP_HEAD:
		(server_->searchHEAD(httpContext->path_))(*httpContext);
		break;

	}
	auto it = httpContext->headers_.find("Connection");
	if (it == httpContext->headers_.end()) {
		if (httpContext->version_ == HttpVersion::HTTP_10) {
			return false;
		}
		return true;
	}
	if (it->second == "Close") {
		return false;
	}
	return true;
}

