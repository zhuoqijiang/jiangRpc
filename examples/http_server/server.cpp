
#include "jiangRpc/net/http/http_server.h"






void echo_helloworld(jiangRpc::HttpContext& ctx)
{
	ctx.string(jiangRpc::HttpStatus::HTTP_OK,"hello world!");
}

int main()
{
	jiangRpc::HttpServer server("localhost", 8080);
	server.GET("/", echo_helloworld);
	
	server.start();
}
