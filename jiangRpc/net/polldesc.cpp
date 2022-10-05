#include "jiangRpc/net/polldesc.h"
#include "jiangRpc/net/scheduler.h"

using namespace jiangRpc;
const uintptr_t pdReady = 1;
const uintptr_t pdWait = 2;
PollDesc::SP_Task* PollDesc::netpollUnblock(int mode)
{
	std::atomic_uintptr_t* tpp = &rtask_;
	
	if (mode == 'w') {
		tpp = &wtask_;
	}
	while (true) {
		uintptr_t oldtask = tpp->load();
		if (oldtask == pdReady) {
			return nullptr;
		}
		uintptr_t newtask = pdReady;

		if (tpp->compare_exchange_strong(oldtask, newtask)) {
			if (oldtask == pdWait || oldtask == 0) {
				return nullptr;
			}
			return reinterpret_cast<SP_Task*> (oldtask);
		}

	}
	
}
bool PollDesc::netpollBlock(int mode)
{
	std::atomic_uintptr_t* tpp = &rtask_;
	if (mode == 'w') {
		tpp = &wtask_;
	}
	
	while (true) {
		uintptr_t zero = 0;
		uintptr_t pdready = pdReady;
		uintptr_t pdwait = pdWait;
		if (tpp->compare_exchange_strong(pdready, zero)) {
			return true;
		}
		if (tpp->compare_exchange_strong(zero, pdwait)) {
			break;
		}
	}
	
	if (netpollBlockCommit(tpp)) {
		t_processor->yield();
	}

	uintptr_t oldt = tpp->exchange(0);

	return oldt == pdReady;
}

bool PollDesc::netpollBlockCommit(std::atomic_uintptr_t* tpp) 
{
	uintptr_t pdwait = pdWait;
	return tpp->compare_exchange_strong(pdwait, reinterpret_cast<uintptr_t> (&task_));
}

void PollDesc::reset()
{
	rtask_.store(0);
	wtask_.store(0);
	task_.reset();
	timer_.reset();
	error_ = false;
}

void PollDesc::waitRead()
{
	setDeadline();
	task_ = t_processor->curTask();
	while (!netpollBlock('r'));
	timer_->del(true);
}

void PollDesc::waitWrite()
{
	task_ = t_processor->curTask();
	while (!netpollBlock('w'));
}

void PollDesc::setDeadline()
{
	if (!deadline_) {
		return;
	}
	Scheduler* sche = &g_scheduler;
	timer_->updateTimer(deadline_);
	std::shared_ptr<Timer>& timer = timer_;
	auto func = [sche, timer]() {
		sche->startTiming(timer);
	};
	t_processor->addFunctor(func);
}
