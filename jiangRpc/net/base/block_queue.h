#pragma once

#include <queue>
#include <condition_variable>
#include <mutex> 
#include <ratio>
#include "boost/noncopyable.hpp"

namespace jiangRpc {

template <typename T>
class BlockQueue: public boost::noncopyable {
public:
	BlockQueue(){};
	bool empty();
	size_t size();
	T pop(bool* status);
	void push(const T&, bool* status);
	void emplace(T&&);
	std::queue<T> popAll();
	T front();
private:
	uint32_t waitTimeMs_ = 1000;
	uint32_t maxSize_ = 128 * 128;
	std::mutex mutex_;
	std::condition_variable emptyCondVar_;
	std::queue<T> queue_;
};

template<typename T> 
bool BlockQueue<T>::empty()
{
	std::unique_lock<std::mutex> locker(mutex_);
	return queue_.empty();
}


template<typename T> 
size_t BlockQueue<T>::size()
{
	std::unique_lock<std::mutex> locker(mutex_);
	return queue_.size();
}

template<typename T> 
T BlockQueue<T>::pop(bool* status)
{
	std::unique_lock<std::mutex> locker(mutex_);
	while (queue_.size() <= 0) {
		if (std::cv_status::timeout == emptyCondVar_.wait_for(locker, std::chrono::milliseconds(waitTimeMs_))) {
			*status = false;
			return nullptr;
		}
	}
	T val = queue_.front();
	queue_.pop();
	*status = true;
	return val;
}

template<typename T> 
void BlockQueue<T>::push(const T& val, bool* status)
{
	std::unique_lock<std::mutex> locker(mutex_);
	//if (queue_.size() >= maxSize_) {
	//	*status = false;
	//	return;
	//}
	*status = true;
	queue_.push(val);
	emptyCondVar_.notify_one();
}

template<typename T> 
void BlockQueue<T>::emplace(T&& val)
{
	std::unique_lock<std::mutex> locker(mutex_);
	queue_.emplace(move(val));
	emptyCondVar_.notify_one();
}

template<typename T> 
T BlockQueue<T>::front()
{
	std::unique_lock<std::mutex> locker(mutex_);
	while(queue_.size() <= 0) {
		emptyCondVar_.wait(locker);
	}
	T val = queue_.front();
	return val;
}
template<typename T>
std::queue<T> BlockQueue<T>::popAll()
{
	std::queue<T> vals;
	std::unique_lock<std::mutex> locker(mutex_);
	while (queue_.size() <= 0) {
		if (std::cv_status::timeout == emptyCondVar_.wait_for(locker, std::chrono::milliseconds(waitTimeMs_))) {
			return vals;
		}
	}
	vals.swap(queue_);
	return vals;
}

}
