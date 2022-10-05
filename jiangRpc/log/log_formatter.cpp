#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "jiangRpc/log/log_formatter.h"
#include "jiangRpc/log/log_event.h"
using namespace jiangRpc;

std::string LogFormatter::LogTime::nowFormatTime()
{
	char str[48];
	memset(str, 0, 48);
	struct timeval tTimeVal;
    gettimeofday(&tTimeVal, NULL);
    struct tm *tTM = localtime(&tTimeVal.tv_sec);
    sprintf(str,"%04d-%02d-%02d %02d:%02d:%02d.%03ld.%03ld", 
    tTM->tm_year + 1900, tTM->tm_mon + 1, tTM->tm_mday, 
    tTM->tm_hour, tTM->tm_min, tTM->tm_sec,             
    tTimeVal.tv_usec / 1000, tTimeVal.tv_usec % 1000);
    return str;
}

std::string LogFormatter::format(const LogEvent& logEvent)
{
	::std::ostringstream ss;
	std::string nowTime = logTime_.nowFormatTime();
	ss << "[" << nowTime  << "] ";
	ss << "[" << logEvent.level_ << "] ";
	ss << "[" << logEvent.threadId_  << "] ";
	ss << "[" <<  logEvent.filePath_ << ":" << logEvent.line_ << "] ";
	ss << "[" << logEvent.funName_  << "] ";
	ss << "[" << logEvent.content_  << "] ";
	ss << std::endl;
	return ss.str(); 
}
