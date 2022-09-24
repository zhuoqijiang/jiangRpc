#pragma once

#include <queue>
#include <condition_variable>
#include <mutex> 
#include "boost/noncopyable.hpp"

template <typename T>
class BlockQueue: public boost::noncopyable {
public:
	BlockQueue(){};
	bool empty();
	size_t size();
	T pop();
	void push(const T&);
	void emplace(T&&);
	T front();

private:
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
T BlockQueue<T>::pop()
{
	std::unique_lock<std::mutex> locker(mutex_);
	while (queue_.size() <= 0) {
		emptyCondVar_.wait(locker);
	}
	T val = queue_.front();
	queue_.pop();
	return val;
}

template<typename T> 
void BlockQueue<T>::push(const T& val)
{
	std::unique_lock<std::mutex> locker(mutex_);
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


