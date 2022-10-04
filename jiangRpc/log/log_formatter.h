#pragma once

#include <stdint.h>
#include <string>
#include <boost/noncopyable.hpp>

namespace jiangRpc {

class LogEvent;

class LogFormatter: public boost::noncopyable {
public:
	
	class LogTime {
	public:
		std::string nowFormatTime();
	};
	
	std::string format(const LogEvent& logEvent);
private:
	LogTime logTime_;
};

}
