#pragma once 

#include <stdint.h>
#include <string>
#include <thread>
#include <map>
#include <boost/noncopyable.hpp>
#include "log_appender.h"

#define g_logger jiangRpc::Logger::GetLogger() 
#define LOG_DEBUG(...) g_logger.log(jiangRpc::Logger::Level::DEBUG,__LINE__,__FILE__,__FUNCTION__,__VA_ARGS__)
#define LOG_INFO(...) g_logger.log(jiangRpc::Logger::Level::INFO,__LINE__,__FILE__,__FUNCTION__,__VA_ARGS__)
#define LOG_WARN(...) g_logger.log(jiangRpc::Logger::Level::WARN,__LINE__,__FILE__,__FUNCTION__,__VA_ARGS__)
#define LOG_ERROR(...) g_logger.log(jiangRpc::Logger::Level::ERROR,__LINE__,__FILE__,__FUNCTION__,__VA_ARGS__)


namespace jiangRpc {

class LogAppender;
class Logger: public boost::noncopyable{
public:
	enum Level {
		DEBUG,
		INFO,
		WARN,
		ERROR
	};
	std::map<Level, std::string>& levelStringMap()
	{
		static std::map<Level, std::string> levels {
			{DEBUG, "DEBUG"},
			{INFO, "INFO"},
			{WARN, "WARN"},
			{ERROR, "ERROR"},
		};
		return levels;
	}

	void setFile(const std::string& filePath)
	{
		appender_.setFile(filePath);
	}

	void setLevel(Level level)
	{
		level_ = level;
	}
	

	void log(Level level, uint32_t line, std::string filePath, std::string funName, std::string fmt, ...);
	
	static Logger& GetLogger(){
		static Logger logger;
		return logger;
	}
	
	~Logger();
private:
	Logger();
	std::string parsefmt(const std::string& fmt, va_list* valist);
private:
	Level level_ = INFO;
	LogAppender appender_;
};

}
