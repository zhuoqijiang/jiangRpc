#pragma once

#include <vector>
#include <memory>
#include <sys/epoll.h>
#include <boost/core/noncopyable.hpp>
#include <boost/noncopyable.hpp>


namespace jiangRpc {

class PollDesc;
class Task;

class Netpoll: public boost::noncopyable {
public:
	using SP_Task = std::shared_ptr<Task>;
	using TaskList = std::vector<SP_Task>;
	Netpoll();

	bool netpollOpen(int fd, PollDesc*);
	
	bool netpollClose(int fd);

	TaskList netpoll(uint32_t timeout);
	TaskList netpollDeadline();
private:
	void netpollReady(TaskList& tList, int mode, PollDesc*);
	void netpollDeadline(TaskList& list, int mode, PollDesc*);
	std::string eventToString(int ev);
private:
	int pollFd_;
	std::vector<epoll_event> events_;
};

}
