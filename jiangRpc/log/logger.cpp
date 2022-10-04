#include <memory>
#include <stdarg.h>
#include "log_event.h"
#include "logger.h" 

using namespace jiangRpc;


Logger::Logger()
{
	appender_.start();
}

Logger::~Logger()
{
	appender_.stop();
}

void Logger::log(Level level, uint32_t line, std::string filePath, std::string funName, std::string fmt, ...){
	if (level < level_) {
		return;
	}
	uint32_t threadId = pthread_self();
	va_list valist;
	va_start(valist, fmt);
	std::string content = parsefmt(fmt, &valist);

	std::shared_ptr<LogEvent> event = std::make_shared<LogEvent>(line, threadId, filePath, levelStringMap()[level], funName, move(content));
	appender_.append(event);
}


std::string Logger::parsefmt(const std::string& fmt, va_list* valist)
{
	std::string content;
	int fmtLength = fmt.length();
	std::string parmString;
	for (int i = 0; i < fmtLength; i++) {
	 	if (i + 1 < fmtLength && fmt[i] == '%'){
			switch (fmt[++i]) {
			case 'd':
				parmString = std::to_string(va_arg(*valist,int));
				content.append(parmString);
				break;
			case 's':
				parmString = std::string(va_arg(*valist,char*));
				content.append(parmString);
				break;
			case 'c':
				content.push_back(static_cast<char>(va_arg(*valist,int)));
				break;
			case 'f':
				parmString = std::to_string(va_arg(*valist,double));
				content.append(parmString);
				break;
			case 'l':
				if (i + 1 < fmtLength) {
					if( fmt[i + 1] == 'f') {
						i++;
						parmString=std::to_string(va_arg(*valist,double));
						content.append(parmString);
					}
					else if (fmt[i+1] == 'd') {
						i++;
						parmString=std::to_string(va_arg(*valist,long long));
						content.append(parmString);
					}
					else
						return "";
				}
				break;
			case '%':
				content.push_back('%');
				break;
			}
			continue;
		}
		content.push_back(fmt[i]);
	}
	return content;
}
