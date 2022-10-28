
#include <pthread.h>
#include <memory>
#include "jiangRpc/net/task.h"
#include "jiangRpc/net/processor.h"
#include "jiangRpc/net/scheduler.h"


using namespace jiangRpc;
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
	//t_processor->bindCpu();
	/*
	while (t_processor->running_) {
		bool status;
		t_processor->curTask_ = t_processor->activeTasks_.pop(&status);
		if (!status) {
			t_processor->curTask_ = g_scheduler.stealWork(&status);
			if (!status) {
				continue;
			}
		}
		t_processor->curTask_->setProcessor(processor);
		if (t_processor->curTask_->getState() == Task::State::DEAD) {
			continue;
		}
		t_processor->resume();
	}
	*/
	while (t_processor->running_) {
		auto tasks = t_processor->activeTasks_.popAll();
		if (tasks.empty()) {
			bool status = false;
			SP_Task task = g_scheduler.stealWork(&status);
			if (!status) {
				continue;
			}
			tasks.push(task);
		}

		while(!tasks.empty()) {
			t_processor->curTask_ = tasks.front();
			tasks.pop();
			t_processor->curTask_->setProcessor(processor);
			if (t_processor->curTask_->getState() == Task::State::DEAD) {
				continue;
			}
			t_processor->resume();
		}
	}
	t_processor->running_ = false;
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

void Processor::bindCpu()
{
	cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(t_processor->cpu_,&cpuset);
	pthread_setaffinity_np(t_processor->thread_.native_handle(), sizeof(cpu_set_t), &cpuset);
}
