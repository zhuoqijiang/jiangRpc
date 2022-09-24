#pragma once

#include <memory>
#include <boost/noncopyable.hpp>
#include "context.h"


class Processor;

class Task: public boost::noncopyable {
public:
	using Functor = std::function<void()>;

	enum State {
		RUN,
		BLOCK,
		DEAD
	};
	Task(const Functor& cb, size_t stackSize, uint64_t id): taskId_(id), ctx_(cb, stackSize){}
	
	void swapIn()
	{
		ctx_.swapIn();
	}

	void swapOut()
	{
		ctx_.swapOut();
	}
	

	State getState()
	{
		return state_;
	}

	void setState(State state)
	{
		state_ = state;
	}
	
	uint64_t getId()
	{
		return taskId_;
	}
	
	void setProcessor(Processor* processor)
	{
		processor_ = processor;
	}

	Processor* getProcessor()
	{
		return processor_;
	}
	
private:
	uint64_t taskId_;
	State state_ = State::BLOCK;
	Context ctx_;
	Processor* processor_;
};

