#pragma once

#include <unistd.h>
#include <memory>
#include <set>
#include <vector>
#include <boost/noncopyable.hpp>

namespace jiangRpc {
class PollDesc;

class Timer: public boost::noncopyable {
public:
	
	Timer(){}

	Timer(int timeout, PollDesc* pd);

	Timer& setPollDesc(PollDesc* pd)
	{
		pd_ = pd;
		return *this;
	}
	

	void updateTimer(int timeout);

	
	bool isExpired();
	
	bool isDel()
	{
		return isDeleted_;
	}

	void del(bool i)
	{
		isDeleted_ = i;
	}

	bool isInQueue()
	{
		return isInQueue_;
	}

	void inQueue(bool i)
	{
		isInQueue_ = i;
	}
	
	bool isValid()
	{
		return isValid_;
	}

	void valid(bool i)
	{
		isValid_ = true;
	}
	PollDesc* polldesc()
	{
		return pd_;
	}
	uint64_t getExpTime()
    {
	    return expiredTime_;
    }
	

public:
	static const int USecond = 1;
	static const int Second = 1000;
	static const int Minute = 1000 * 60;
	static const int Hour = 1000 * 60 * 60;
	static const int Day = 1000 * 60 * 60 * 24;
private:
	PollDesc* pd_;
	uint64_t expiredTime_;
	bool isDeleted_ = false;
	bool isInQueue_ = false;
	bool isValid_ = false;
};


struct TimerCmp {
	using Timer_PTR = std::shared_ptr<Timer>;
  	bool operator()(const Timer_PTR &a, const Timer_PTR &b) const
    {
    	return a->getExpTime() < b->getExpTime();
  	}
};


class TimerQueue {
public:
	bool addTimer(const std::shared_ptr<Timer>& timer);
	std::vector<PollDesc*> getExpiredPolldescs();
private:
  	std::set<std::shared_ptr<Timer>, TimerCmp> timers_;
};

}
