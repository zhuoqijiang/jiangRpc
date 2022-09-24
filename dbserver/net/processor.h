#pragma once

#include <memory>
#include <thread>
#include <functional>
#include <boost/noncopyable.hpp>
#include "base/block_queue.h"
#define t_processor Processor::GetProcessor()


class Task;

class Processor: public boost::noncopyable{
public:
	using Functor = std::function<void()>;
	using SP_Task = std::shared_ptr<Task>;
	Processor() = default;
	Processor(int cpu):cpu_(cpu) {}
	void start();
	void join();
	void stop();

	void addTask(SP_Task& task)
	{
		activeTasks_.push(task);
	}
	uint64_t curTaskId();
	SP_Task& curTask();
	
	
	void addFunctor(const Functor& func);

	std::vector<Functor> getFunctors();
public:
	static Processor*& GetProcessor()
	{
		static thread_local Processor* processor = nullptr;
		return processor;
	}
	static void Process (Processor*);

	void yield();
	void resume();

private:
	void fillExpiredTasks();
private:
	bool running_ = false;
	
	SP_Task curTask_;
	BlockQueue<SP_Task> activeTasks_; 
	
	std::mutex funcsMutex_;
	std::vector<Functor> funcs_;
	std::thread::id id_;
	std::thread thread_;	

	int cpu_;
};
