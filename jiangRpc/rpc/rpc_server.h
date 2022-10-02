#pragma once
#include "jiangRpc/net/http/http_server.h"
#include "jiangRpc/net/http/http_enum.h"
#include "traits.h"

namespace jiangRpc {

class RpcServer {
public:
	RpcServer(std::string addr, int port): httpServer_(addr, port){}

	template<typename Function>
	bool registService(std::string name, Function func)
	{
		if (function_traits<Function>::arg_size != 1) {
				return false;
		}
		httpServer_.GET(name, FuncTohttpRouter(func));
		return true;
	}

	void start()
	{
		httpServer_.start();
	}
	
	void setDeadline(int deadline)
	{
		httpServer_.setDeadline(deadline);
	}

	void setWorkNum(int workNum)
	{
		httpServer_.setWorkerNum(workNum);
	}

private:

	template<typename Function>
	HttpServer::Functor FuncTohttpRouter(Function fun)
	{
		return [fun](HttpContext& ctx)-> void{
			std::string input = ctx.body();
			typename function_traits<Function>::functional_type func = fun;
			
			typename function_traits<Function>::template args<0>::type req;
			typename function_traits<Function>::return_type resp;
			
			if (!req.ParseFromString(input)) {
				ctx.string(HttpStatus::HTTP_BAD_REQUEST, "BAD REQUEST");
				return;
			}
	
			resp = func(req); 
			std::string resp_body;
			if (!resp.SerializeToString(&resp_body)) {
				ctx.string(HttpStatus::HTTP_BAD_REQUEST, "BAD REQUEST");
				return;
			}
			ctx.string(HttpStatus::HTTP_OK, resp_body);
		
		};
	}
private:
	HttpServer httpServer_;
};
}
