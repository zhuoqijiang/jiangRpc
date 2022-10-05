#pragma once

#include "jiangRpc/net/polldesc.h"
#include <boost/core/noncopyable.hpp>

int setNonblock(int fd);
namespace jiangRpc {

struct Address {
	int port;
	std::string addr;
	Address(){}
	Address(const std::string addr, int port)
	:port(port), addr(addr){}
};



class NetConnFd: public boost::noncopyable{
public:
	bool netOpen(int fd, const Address& laddr, const Address& raddr);
	int netRead(std::string& str);
	int netWrite(std::string& str);
	bool netClose();
	
	Address getLaddr()
	{
		return laddr_;
	}
	Address getRaddr()
	{
		return raddr_;
	}
	
	void setDeadline(int timeout)
	{
		pd_.initDeadline(timeout); 
	}

private:
	bool netpollOpen();
	bool netpollClose();
	int nonblockRead(std::string& str);
	int nonblockWrite(std::string& str);
	bool checkError()
	{
		return pd_.getError();
	}
private:
	int fd_;
	Address laddr_;
	Address raddr_;
	PollDesc pd_;
};




class NetAcceptorFd: public boost::noncopyable{
public:
	NetAcceptorFd(const std::string& addr, int port)
	:addr_(addr, port)
	{}
	void setDeadline(int timeout)
	{
		deadline_ = timeout;
	}
	bool netAccept(NetConnFd& NetFd);
	bool socketBindListen();
private:
	int fd_;
	int deadline_ = 0;
	Address addr_;
};
}
