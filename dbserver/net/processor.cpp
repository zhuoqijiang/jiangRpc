
#include <pthread.h>
#include <memory>
#include "task.h"
#include "processor.h"
#include "scheduler.h"
#include <iostream>
void Processor::start()
{
	running_ = true;
	thread_ = std::thread(Processor::Process, this);
	id_ = thread_.get_id();
}

void Processor::join()
{
	thread_.join();
}

void Processor::stop()
{
	running_ = false;
}

void Processor::Process(Processor* processor)
{
	t_processor = processor;
	/*
	cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(t_processor->cpu_,&cpuset);

	pthread_setaffinity_np(t_processor->thread_.native_handle(), sizeof(cpu_set_t), &cpuset);
	*/
	while (t_processor->running_) {
		t_processor->curTask_ = t_processor->activeTasks_.pop();
		if (t_processor->curTask_->getState() == Task::State::DEAD) {
			continue;
		}
		t_processor->resume();
	}
	t_processor->running_ = false;
}


uint64_t Processor::curTaskId()
{
	return curTask_->getId();
}


Processor::SP_Task& Processor::curTask()
{
	return curTask_;
}

void Processor::addFunctor(const Functor& func)
{
	std::lock_guard<std::mutex> locker(funcsMutex_); 
	funcs_.push_back(func);
}

std::vector<Processor::Functor> Processor::getFunctors()
{
	std::vector<Functor> funcs;
	{
	std::lock_guard<std::mutex> locker(funcsMutex_);
	funcs_.swap(funcs);
	}
	return funcs;
}



void Processor::yield()
{
	curTask_->setState(Task::State::BLOCK);
	curTask_->swapOut();
}


void Processor::resume()
{
		
	curTask_->setState(Task::State::RUN);
	curTask_->swapIn();
}
