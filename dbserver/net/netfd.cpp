#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <string>
#include "netfd.h"
#include "scheduler.h"
int setNonblock(int fd)
{
	int iFlags;
    iFlags = fcntl(fd, F_GETFL, 0);
    iFlags |= O_NONBLOCK;
    iFlags |= O_NDELAY;
    int ret = fcntl(fd, F_SETFL, iFlags);
    return ret;
}

bool NetConnFd::netpollOpen()
{
	return g_scheduler.netpollOpen(fd_, pd_);
}

bool NetConnFd::netpollClose()
{
	return g_scheduler.netpollClose(fd_);
}


bool NetConnFd::netOpen(int fd, const Address& laddr, const Address& raddr)
{
	fd_ = fd;
	laddr_ = laddr;
	raddr_ = raddr;
	if (setNonblock(fd_) < 0) {
		return false;
	}
	if (!netpollOpen()) {
		return false;
	}
	return true;
}

bool NetConnFd::netClose()
{
	if (!netpollClose()) {
		return false;
	}
	pd_.reset();
	close(fd_);
	return true;
}

int NetConnFd::netRead(std::string& str)
{
	while (true) {
		int n = nonblockRead(str);
		if (n == -1) {
			return 0;
		}
		if (n == 0) {
			pd_.waitRead();
			if (checkError()) {
				return 0;
			}
			continue;
		}
		return n;
	}
}

int NetConnFd::netWrite(std::string& str)
{
	while (true) {
		int n = nonblockWrite(str);
		if (n == -1) {
			return 0;
		}

		if (n == 0) {
			pd_.waitWrite();
			continue;
		}
		if (checkError()) {
				return 0;
		}
		return n;
	}
}

int NetConnFd::nonblockRead(std::string& str)
{
    int length = 65536;
    int nowReadPos = 0, n = 0;
    while (1) {
        str.resize(length);
        n = read(fd_, static_cast<char*>(&str[nowReadPos]), length - nowReadPos);
        if ( n < 0 ) {
            if (errno == EAGAIN) {
                break;
            }
            if (errno == EINTR) {
                continue;
            }
			if (errno == SIGPIPE){
				return -1;
			}
            return -1;
        }
        else if (n == 0) {
            return -1;
        }
        length *= 2;
        nowReadPos += n;
    }
    return nowReadPos;

}

int NetConnFd::nonblockWrite(std::string& str)
{
    int length = str.size();
    int nowWritePos = 0, n = 0;
    while (nowWritePos < length) {
         n = write(fd_, static_cast<char*>(&str[nowWritePos]), length - nowWritePos);
         if ( n < 0 ) {
             if (errno == EAGAIN) {
                 break;
             }
             if (errno == EINTR) {
                 continue;
             }
			 if (errno == SIGPIPE) {
				 return -1;
			 }
             return -1;
         }
         else if (n == 0) {
             return -1;
         }
         nowWritePos += n;
     }
     return nowWritePos;
}

bool NetAcceptorFd::netAccept(NetConnFd& netConnFd)
{
	sockaddr_in addr;
	socklen_t len = sizeof(sockaddr_in);
	int fd = accept(fd_, (sockaddr*)&addr, &len);
	Address raddr;
	raddr.addr = inet_ntoa(addr.sin_addr);
	raddr.port = ntohs(addr.sin_port);
	
	if (!netConnFd.netOpen(fd, addr_ , raddr)) {
		close(fd);
		return false;
	};
	if (deadline_) {
		netConnFd.setDeadline(deadline_);
	}
	return true;
}


bool NetAcceptorFd::socketBindListen()
{  
	if ((fd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return false;
	}
    struct sockaddr_in server_addr;
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	if (addr_.addr != "") {
		server_addr.sin_addr.s_addr=htonl(atoi(addr_.addr.c_str()));
	}
    server_addr.sin_port = htons(addr_.port);
    if(bind(fd_, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		return false;
    }
    if(listen(fd_, 4096) < 0){
		return false;
    }
	return true;
}


