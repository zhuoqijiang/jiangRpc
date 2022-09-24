#pragma once

#include <functional>
#include <stdlib.h>
#include <memory>

#include <boost/context/detail/fcontext.hpp>
#include <boost/noncopyable.hpp>
#define t_context Context::GetTlsContext()
using namespace boost::context::detail;


class Context: public boost::noncopyable {
public:
	using Functor = std::function<void()> ;
	
	Context(){}

	Context(const Functor& cb, size_t stackSize): cb_(cb), stackSize_(stackSize)
	{
		while (1) {
			stack_ = (char*)malloc(stackSize_);
			if (stack_ == nullptr) {
				continue;
			}
			break;
		}
		ctx_ = make_fcontext(stack_ + stackSize_, stackSize_, Context::Fun);
	}

	~Context()
	{
		if (stack_ != nullptr)
			free(stack_);
		stack_ = nullptr;
	}

	void swapIn()
	{
		auto transfer = jump_fcontext(ctx_, this);
		ctx_ = transfer.fctx;
	}


	void swapOut()
	{
		auto transfer = jump_fcontext(t_context.ctx_, this);
		t_context.ctx_ = transfer.fctx;
	}

	
	static Context& GetTlsContext()
	{
		static thread_local Context localCtx;
		return localCtx;
	}
	
	static void Fun(transfer_t transfer)
	{
		Context* ctx = static_cast<Context*>(transfer.data);

		t_context.ctx_ = transfer.fctx;
		
		ctx->cb_();
		
		ctx->swapOut();
	}

private:
	Functor cb_;
	size_t stackSize_ = 0;
	char* stack_ = nullptr;
	fcontext_t ctx_ = nullptr;
};

