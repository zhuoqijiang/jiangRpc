#pragma once
#include <boost/core/noncopyable.hpp>
#include <memory>
#include <atomic>
#include <boost/noncopyable.hpp>
#include "timer.h"

namespace jiangRpc {
class Task;
class PollDesc: public boost::noncopyable {
public:
	using SP_Task = std::shared_ptr<Task>;
	
	PollDesc()
	{
		timer_ = std::make_shared<Timer>();
		timer_->setPollDesc(this);	
	}
	void initDeadline(int timeout)
	{
		deadline_ = timeout;
	}
	void setTask(const SP_Task& task)
	{
		task_ = task;
	}

	
	void reset();
	void setError(bool error)
	{
		error_ = error;
	}
	bool getError()
	{
		return error_;
	}
	void waitRead();
	void waitWrite();
	
	SP_Task* netpollUnblock(int mode);
	bool netpollBlock(int mode);
private:
	
	void setDeadline();
	bool netpollBlockCommit(std::atomic_uintptr_t* tpp);
private:
	SP_Task task_;
	std::atomic_uintptr_t rtask_;
	std::atomic_uintptr_t wtask_;

	std::shared_ptr<Timer> timer_;
	int deadline_ = 0;
	bool error_ = false;
	
};

}
