
#include <sys/time.h>
#include "timer.h"

Timer::Timer(int timeout, PollDesc* pd)
:pd_(pd) 
{
	struct timeval now;
	gettimeofday(&now, NULL);
	expiredTime_ =
    (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}


bool Timer::isExpired()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	uint64_t nowTime =
    (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
	

	return nowTime >= expiredTime_;
}

void Timer::updateTimer(int timeout)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	expiredTime_ = 
		(((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;

}

bool TimerQueue::addTimer(const std::shared_ptr<Timer> &timer)
{
	timer->del(false);
	if (timer->isInQueue()) {
		timer->valid(true);
		return true;
	}
	timers_.insert(timer);
	timer->inQueue(true);
	return true;
}



std::vector<PollDesc*> TimerQueue::getExpiredPolldescs()
{
	std::vector<PollDesc*> pds;
	
	for (auto iter = timers_.begin(); iter != timers_.end();) {
		std::shared_ptr<Timer> timer = *iter;
		auto nextIter = prev(iter, -1);
		if (timer->isExpired()) {
			timers_.erase(iter);
			if (!timer->isDel()) {
				pds.push_back(timer->polldesc());
			}
			timer->valid(false);
			timer->inQueue(false);
			iter = nextIter;
			continue;
		}
		if (timer->isValid()) {
			iter = nextIter;
			continue;
		}
		break;
	}
	
	return pds;
}

