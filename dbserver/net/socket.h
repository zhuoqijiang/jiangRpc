
#include <functional>
#include <memory>
#include "netfd.h"
#include "scheduler.h"
#include "signal.h"


class Conn {
public:
	
	friend class Acceptor;

	int read(std::string& str)
	{
		return fd_.netRead(str);
	}
	
	int write(std::string& str)
	{
		return fd_.netWrite(str);
	}
	
	void close()
	{
		fd_.netClose();
	}
	
	Address getLocalAddress()
	{
		return fd_.getLaddr(); 
	}

	Address getRemoteAddress()
	{
		return fd_.getRaddr();
	}
private:
	NetConnFd fd_;
};



#include <iostream>
class Acceptor {
public:
	Acceptor(std::string addr, int port, const std::function<void(Conn&)>& handle)
	:fd_ (addr, port), handle_(handle){}
	
	Acceptor& setDeadline(int timeout)
	{
		fd_.setDeadline(timeout);
		return *this;
	}

	void start()
	{
		if (!fd_.socketBindListen()) {
			 std::cerr <<  "socket bind error" << std::endl;
			 abort();
		}
		signal(SIGPIPE, SIG_IGN);
		while (true) {
			std::shared_ptr<Conn> conn = std::make_shared<Conn>(); 

			if (!fd_.netAccept(conn->fd_)) {
				return;
			}
			auto lam = [this ,conn](){
				this->handle_(*conn);	
			};
			g_scheduler.createTask(lam);
		}
	}

	
private:
	NetAcceptorFd fd_;
	std::function<void(Conn&)> handle_;
};
