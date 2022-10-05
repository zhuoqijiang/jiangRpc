#pragma once

#include <memory>
#include <thread>
#include <iostream>
#include <fstream>
#include "jiangRpc/log/log_buffer.h"
#include "jiangRpc/log/log_formatter.h"
#include "jiangRpc/log/log_event.h"

namespace jiangRpc {


class LogAppender {
public:
	void start()
	{
		auto rstart = [this]() {
		while (logging_) {
			std::string buffer;
			auto events = logBuffer_.achieve();
			for (auto event: events) {
				buffer += logFormatter_.format(*event);
			}
			(*os_) << buffer;
			os_->flush();
		}
		};

		thread_ = std::thread(rstart);
	}
	
	void stop()
	{
		logging_ = false;
		thread_.join();
	}
	void append(std::shared_ptr<LogEvent> logEvent)
	{
		logBuffer_.append(logEvent);
	}

	void setFile(const std::string& filePath) 
	{
		if (os_ != &std::cout) {
			dynamic_cast<std::ofstream*>(os_)->close();
			delete os_;
		} 
		os_ = new std::ofstream(filePath, std::ios_base::app);
	}

private:
	bool logging_ = true;
	LogBuffer  logBuffer_;
	LogFormatter logFormatter_; 
	std::thread thread_;
	std::ostream* os_ = &std::cout;
};

}
