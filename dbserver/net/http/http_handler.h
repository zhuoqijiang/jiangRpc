#pragma once
#include <string>
#include <memory>

#include "http_context.h"
#include "http_server.h"
class Conn;
//class HttpContext;

class HttpHandler{
public:
	friend class HttpContext;
	HttpHandler(HttpServer* server, Conn* conn);
	HttpHandler() = default; // for test
	~HttpHandler();
	void handle();
private:
	ParseState parse(HttpContext* httpContext);

	ParseState parseLine(HttpContext* httpContext);
	bool parseQuery(HttpContext* httpContext,const std::string& query);
	bool checkKeyName(const std::string key); 

	ParseState parseHeaders(HttpContext* httpContext);
	ParseState parseBody(HttpContext* httpContext);

	bool analyse(HttpContext* httpContext);
private:
	HttpServer* server_;
	Conn* conn_;
	std::string buffer_;
	int nowReadPos_ = 0;
};
