// 
// simple log implementation
// Version:1.0
// Copyright 2017 James Wei (weijianlhp@163.com)
//

#include <stdarg.h>
#include <sys/timeb.h>
#include <string>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include "log.h"

namespace cloris {

static LogLevel g_log_level = LogLevel::INFO;

void FormatOutput(const char *category, LogLevel level, const char *file_name, int line_num, const char *format, ...) {
    if (level < g_log_level) {
        return;
    }
    int size = 2048;
    char *buf = NULL;
    char *new_buf = NULL;
    buf = (char *)malloc(size);
    if (!buf) {
        return;
    }
    va_list ap;
    int n;
    while (true) {
        va_start(ap, format);
        n = vsnprintf(buf, size, format, ap);
        va_end(ap);
        if (n > -1 && n < size) {
            break;
        }
        if (n > -1) {
            size = n + 1;
        } else {
            size = size << 1;
        }
        new_buf = (char*)realloc(buf, size);
        if (!new_buf) {
            free(buf);
            return;
        } else {
            buf = new_buf;
        }
    }
    std::string str_level;
    if (level == TRACE) {
        str_level = "TRACE";
    } else if (level == DEBUG) {
        str_level = "DEBUG";
    } else if (level == INFO) {
        str_level = "INFO";
    } else if (level == WARN) {
        str_level = "WARN";
    } else if (level == ERROR) {
        str_level = "ERROR";
    } else if (level == FATAL) {
        str_level = "FATAL";
    }

    time_t timenow;
    struct timeb  tmb;
    struct tm     tmstru;
    time(&timenow);
    localtime_r(&timenow, &tmstru);
    ftime(&tmb);

    std::string message = boost::str(
            boost::format("[%s] %2d-%02d-%02d %02d:%02d:%02d:%03d [%s] [%d] <%s:%d>\t")
            % category
            % (tmstru.tm_year + 1900)
            % (tmstru.tm_mon + 1)
            % tmstru.tm_mday
            % tmstru.tm_hour
            % tmstru.tm_min 
            % tmstru.tm_sec
            % tmb.millitm
            % str_level
            % getpid()
            % file_name
            % line_num
            ); 
    message += buf;

    // TODO support more output format and methods 
    std::cout << message << std::endl;
}

} // namespace cloris
