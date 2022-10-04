#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <ratio>
#include <boost/noncopyable.hpp>

namespace jiangRpc {


class LogEvent;
class LogBuffer: public boost::noncopyable {
public:

	void setCacheSize(size_t cacheSize)
	{
		cacheSize_ = cacheSize;
	}
	
	void setWaitTimeMs(uint32_t waitTimeMs) {
		waitTimeMs_ = waitTimeMs;
	}

	void append(std::shared_ptr<LogEvent> logEvent)
	{
		std::unique_lock<std::mutex> locker(reventsMutex_);
		revents_.push_back(logEvent);	
		if (revents_.size() < cacheSize_) {
			return;
		}
		reventsCondVar_.notify_one();
	}
	
	std::vector<std::shared_ptr<LogEvent>> achieve()
	{
		std::vector<std::shared_ptr<LogEvent>> wevents;
		std::unique_lock<std::mutex> locker(reventsMutex_);
		while (revents_.size() <= 0) {
			reventsCondVar_.wait_for(locker, std::chrono::milliseconds(waitTimeMs_));
		}
		wevents.swap(revents_);
		return wevents;
	}
private:
	size_t cacheSize_ = 100;
	uint32_t waitTimeMs_ = 1000 * 2; 
	std::mutex reventsMutex_;
	std::condition_variable reventsCondVar_; 
	std::vector<std::shared_ptr<LogEvent>> revents_;
};

}
