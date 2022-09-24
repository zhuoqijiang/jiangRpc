#pragma once
#include <functional>
#include <memory>
#include <string>
#include "../base/trie.h"
#include "http_context.h"

class HttpHandler;
class HttpServer {
public:
	//friend class HttpHandler;
	using Functor = std::function<void(HttpContext&)>;
	HttpServer() = default;
	HttpServer(std::string addr, int port);
	void start();
	void GET(std::string path, const Functor& cb)
	{
		GET_trie_.insert(path, cb);
	}
	void POST(std::string path, const Functor& cb)
	{
		POST_trie_.insert(path, cb);
	}
	void HEAD(std::string path, const Functor& cb)
	{
		HEAD_trie_.insert(path, cb);
	}
	void setNoRoute(const Functor& cb)
	{
		noRoute_ = cb;
	}
	void setError(const Functor& cb)
	{
		error_ = cb;
	}
	Functor getNoRoute()
	{
		return noRoute_;
	}
	Functor getError()
	{
		return error_;
	}
	HttpServer& setDeadline(int timeout) 
	{
		deadline_ = timeout;
		return *this; 
	}
	
	HttpServer& setWorkerNum(int workerNum)
	{
		workerNum_ = workerNum;
		return *this;
	}
	Functor searchGET(std::string path);
	Functor searchPOST(std::string path);
	Functor searchHEAD(std::string path);
private:
	Functor deFaultNoRouteFunctor();
	Functor deFaultErrorFunctor();
private:
	int workerNum_ = 1;
	std::string addr_ = "localhost";
	int port_ = 8080;
	bool listening_ = false;
	
	int deadline_ = 0;
	Trie<Functor> GET_trie_;
	Trie<Functor> POST_trie_;
	Trie<Functor> HEAD_trie_;
	Functor noRoute_ = deFaultNoRouteFunctor();
	Functor error_ = deFaultErrorFunctor();
};
