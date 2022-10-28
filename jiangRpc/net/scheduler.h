#pragma once 
#include <memory>
#include <functional>
#include <thread>
#include <boost/noncopyable.hpp>

#include "jiangRpc/log/logger.h"
#include "jiangRpc/net/timer.h"
#include "jiangRpc/net/netpoll.h"
#include "jiangRpc/net/base/block_queue.h"
#include "jiangRpc/net/processor.h"

namespace jiangRpc {
class Task;
class NetConnFd;
class PollDesc;

#define g_scheduler Scheduler::GetScheduler()
#define DEFAULT_STACK_SIZE 32 * 1024

class Scheduler: public boost::noncopyable {
public:
	using Functor = std::function<void()>;
	using SP_Task = std::shared_ptr<Task>;
	using UP_Processor = std::unique_ptr<Processor>;
	void start(int threadNum);
	void join();
	void createTask(const Functor& cb, int stackSize = DEFAULT_STACK_SIZE);
	std::vector<SP_Task> getTasks(int num);
	void listen();
	
	static Scheduler& GetScheduler()
	{
		static Scheduler scheduler; 
		return scheduler;
	}

	bool netpollOpen(int fd, PollDesc& pd)
	{
		return netpoll_.netpollOpen(fd, &pd);
	}

	bool netpollClose(int fd)
	{
		return netpoll_.netpollClose(fd);
	}
	

	void startTiming(const std::shared_ptr<Timer>& timer);
	void handleDeadline();
	std::vector<PollDesc*> timer_wait();
	
	SP_Task stealWork(bool* status);

	void handleFunctor();
private:
	Scheduler() {};
	~Scheduler() {};

	uint64_t nextTaskId()
	{
		static uint64_t idCount = 0;
		return ++idCount;
	}
	
	int cycleNextProcessor();
	
	void handlePoll(int timeout);
private:
	std::vector<UP_Processor> processors_;
	
	Netpoll netpoll_;
	BlockQueue<SP_Task> tasks_;
	TimerQueue timers_;

	std::thread m_listenThread;
	bool m_listening = false;
	
};

}
