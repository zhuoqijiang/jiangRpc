#include <iostream>

#include <signal.h>
#include "net/http/http_server.h"
//#include "net/socket.h"
//#include "scheduler.h"
using namespace std;



void test(HttpContext& ctx)
{
	
	ctx.string(HttpStatus::HTTP_OK, ctx.localIp() + " " + ctx.clientIp());
}


void testRequery(HttpContext& ctx)
{
	auto querys = ctx.querys();
	
	std::string resp;
	for (auto query: querys) {
		resp += query.second;
		resp += " ";
	}
	resp += "\n";
	ctx.string(HttpStatus::HTTP_OK, resp);
}

int main()
{
	HttpServer server("localhost", 8080);

	server.GET("/", test);
	server.GET("/testRequery", testRequery);
	server.start();
}
