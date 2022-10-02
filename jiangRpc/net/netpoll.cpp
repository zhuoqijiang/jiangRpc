
#include <sstream>
#include "netpoll.h"
#include "polldesc.h"
#include "scheduler.h"

using namespace jiangRpc;
Netpoll::Netpoll()
{
	pollFd_ = epoll_create(2048);
	events_.resize(128);
}

bool Netpoll::netpollOpen(int fd, PollDesc* pd)
{
	epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
	
	ev.data.ptr = static_cast<void*> (pd);
	if (epoll_ctl(pollFd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
		return false;
	}
	return true;
}

bool Netpoll::netpollClose(int fd)
{
	epoll_event ev;
	if (epoll_ctl(pollFd_, EPOLL_CTL_DEL, fd, &ev) < 0) {
		return false;
	}
	return true;
}

Netpoll::TaskList Netpoll::netpoll(uint32_t timeout)
{
	
	int n = epoll_wait(pollFd_, &*events_.begin(), static_cast<int> (sizeof(events_.size())), timeout);

	TaskList tList;
	epoll_event ev;
	int mode = 0;
	PollDesc* pd = nullptr;  
	for (int i = 0; i < n; i++) {
		ev = events_[i];

		if (ev.events == 0) {
			continue;
		}
		
		if (ev.events & (EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
			mode += 'r';
		}
		
		if (ev.events & (EPOLLOUT | EPOLLHUP | EPOLLERR)) {
			mode += 'w';
		}
		
		if (mode) {
			pd = static_cast<PollDesc*> (ev.data.ptr);
			netpollReady(tList, mode, pd);

			pd->setError(ev.events & EPOLLERR);
		}
	}
	
	return tList;
}




Netpoll::TaskList Netpoll::netpollDeadline()
{
	TaskList tList;
	std::vector<PollDesc*> pds = g_scheduler.timer_wait();

	for (auto pd: pds) {
		netpollDeadline(tList, 'r', pd);
	}
	return tList;
}

void Netpoll::netpollReady(TaskList& tList, int mode, PollDesc* pd)
{
	SP_Task* task;
	if (mode == 'r' || mode == 'r' + 'w') {
		task = pd->netpollUnblock('r');
		if (task != nullptr) {
			tList.push_back(*task);
			return;
		}
	}

	if (mode == 'w' || mode == 'r' + 'w') {
		task = pd->netpollUnblock('w');
		if (task != nullptr) {
			tList.push_back(*task);
			return;
		}
	}

}

void Netpoll::netpollDeadline(TaskList& tList, int mode, PollDesc* pd) {
	SP_Task* task;
	if (mode == 'r' || mode == 'r' + 'w') {
		task = pd->netpollUnblock('r');
		if (task != nullptr) {
			pd->setError(true);
			tList.push_back(*task);
			return;
		}
	}

}

std::string Netpoll::eventToString(int ev)
{
    std::ostringstream oss;
    if (ev & EPOLLIN)
        oss << "IN ";
    if (ev & EPOLLPRI)
        oss << "PRI ";
    if (ev & EPOLLOUT)
        oss << "OUT ";
    if (ev & EPOLLHUP)
        oss << "HUP ";
    if (ev & EPOLLRDHUP)
        oss << "RDHUP ";
    if (ev & EPOLLERR)
        oss << "ERR ";
    return oss.str();
}

