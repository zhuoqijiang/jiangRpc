#pragma once

#include <stdint.h>
#include <string>
#include <boost/noncopyable.hpp>

namespace jiangRpc {


class LogEvent: public boost::noncopyable {
public:
	friend class LogFormatter;
	LogEvent(uint32_t line, uint32_t threadId, const std::string& filePath, const std::string& level, const std::string& funName, std::string&& content)
	:line_(line), threadId_(threadId), filePath_(filePath), level_(level), funName_(funName), content_(content)
	{

	}

private:
	uint32_t line_;
	uint32_t threadId_;
	uint64_t time_;
	std::string filePath_;
	std::string level_;
	std::string funName_;
	std::string content_;
};

}
