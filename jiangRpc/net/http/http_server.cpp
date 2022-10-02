
#include <netinet/in.h>
#include <signal.h>
#include <string>
#include "http_server.h"
#include "http_handler.h"
#include "jiangRpc/net/scheduler.h"
#include "jiangRpc/net/socket.h"

using namespace jiangRpc;
HttpServer::HttpServer(::std::string addr, int port)
: addr_(addr), port_(port)
{
}




void HttpServer::start()
{
	g_scheduler.start(workerNum_);

	
	auto httpHandle = [this](Conn& conn) {
		HttpHandler handler(this, &conn);
		handler.handle();
	};

	Acceptor acceptor(addr_, port_, httpHandle);
	acceptor.start();


	g_scheduler.join();

}

HttpServer::Functor HttpServer::searchGET(::std::string path)
{
	auto getIter = GET_trie_.search(path);
	if (getIter.second) {
		return getIter.first;
	}
	return noRoute_;
}

HttpServer::Functor HttpServer::searchPOST(::std::string path)
{
	auto postIter = POST_trie_.search(path);
	if (postIter.second) {
		return postIter.first;
	}
	return noRoute_;
}

HttpServer::Functor HttpServer::searchHEAD(::std::string path)
{
	auto headIter = HEAD_trie_.search(path);
	if (headIter.second) {
		return headIter.first;
	}
	return noRoute_;
}


HttpServer::Functor HttpServer::deFaultNoRouteFunctor()
{
	return [](HttpContext& httpcontext) {
		httpcontext.string(HttpStatus::HTTP_NOT_FOUND, "404 page no found");
	};
}
HttpServer::Functor HttpServer::deFaultErrorFunctor()
{
	return [](HttpContext& httpcontext) {
		httpcontext.string(HttpStatus::HTTP_BAD_REQUEST, "400 bad request");
	};
}

