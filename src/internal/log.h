// 
// Simple log implementation
// Version:1.0
// Copyright (C) 2017 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_LOG_H_
#define CLORIS_LOG_H_

#define BUG_ON(args) 
#define BUG(args)

#include "internal/def.h"

#ifdef USE_DEBUG 
    #define cLog(level, format, ...) \
        FormatOutput("CLORIS", level, __FILE__, __LINE__, format, ##__VA_ARGS__)
    #define cLogIf(cond, level, format, ...) \
        do {  \
            if (cond) { FormatOutput("CLORIS", level, __FILE__, __LINE__, format, ##__VA_ARGS__); } \
        } while (0) 
#else 
    #define cLog(level, format, ...) \
        void(0)
    #define cLogIf(cond, level, format, ...) \
        void(0)
#endif

namespace cloris {

enum LogLevel {
    TRACE = 1,
    DEBUG = 2,
    INFO  = 3,
    WARN  = 4,
    ERROR = 5,
    FATAL = 6,
};

void FormatOutput(const char *category, LogLevel level, const char *file_name, int line_num, const char *format, ...); 

} // namespace cloris

#endif // CLORIS_LOG_H_
