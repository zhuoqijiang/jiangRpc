

#include <sys/sysinfo.h>

#include "scheduler.h"
#include "task.h"

using namespace jiangRpc;
void Scheduler::start(int threadNum)
{
	m_listening = true;
	
	int cpuNum =  get_nprocs();

	m_listenThread = std::thread(&Scheduler::listen, this);
	if (threadNum < 1) {
		threadNum = cpuNum;
	}
	for (int i = 0; i < threadNum; i++) {
		processors_.push_back(std::make_unique<Processor>(i % cpuNum));
		processors_[i]->start();
	}
}

void Scheduler::join()
{
	m_listening = false;
	m_listenThread.join();
	for (auto& processor: processors_) {
		processor->stop();
		processor->join();
	}
}


void Scheduler::createTask(const Functor& cb, int stackSize)
{
	Functor callback = [cb](){
		cb();
		t_processor->curTask()->setState(Task::State::DEAD);	
	};
	SP_Task task = std::make_shared<Task>(cb, stackSize, nextTaskId());
	Processor* p = processors_[cycleNextProcessor()].get();
	task->setProcessor(p);
	p->addTask(task);
}

void Scheduler::listen()
{
	const int pollTimeout = 1000;
	while (m_listening) {
		handlePoll(pollTimeout);
	    handleDeadline();
	    handleFunctor();
	}
	m_listening = false;
}



void Scheduler::startTiming(const std::shared_ptr<Timer>& timer)
{
	timers_.addTimer(timer);
}


void Scheduler::handlePoll(int timeout)
{
	std::vector<SP_Task> tasks;
	tasks = netpoll_.netpoll(timeout);
	
	for (auto task: tasks) {
		Processor* p = task->getProcessor();
		p->addTask(task);
	}
	
}

void Scheduler::handleDeadline()
{
	std::vector<SP_Task> tasks;
	tasks = netpoll_.netpollDeadline();
	
	for (auto task: tasks) {
		Processor* p = task->getProcessor();
		p->addTask(task);
	}
	
}

void Scheduler::handleFunctor()
{
	std::vector<std::function<void()>> funcs;
	for (auto& p: processors_) {
		funcs = p->getFunctors();
		for (auto func: funcs) {
			func();
		}
	}
}



std::vector<PollDesc*> Scheduler::timer_wait()
{
	return timers_.getExpiredPolldescs();
}



int Scheduler::cycleNextProcessor()
{
	static int cycleCount = -1;
	int pSize = processors_.size();
	cycleCount++;
	if (cycleCount >= pSize) {
		cycleCount = 0;
	}
	return cycleCount;
}




