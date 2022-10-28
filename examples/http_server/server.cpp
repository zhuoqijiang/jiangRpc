
#include "jiangRpc/net/http/http_server.h"
#include "jiangRpc/log/logger.h"







void echo_helloworld(jiangRpc::HttpContext& ctx)
{
	ctx.string(jiangRpc::HttpStatus::HTTP_OK,"hello world!");
}

int main()
{
	g_logger.setLevel(jiangRpc::Logger::Level::ERROR);
	jiangRpc::HttpServer server("localhost", 8080);
	server.GET("/", echo_helloworld);
	
	server.start();
}
