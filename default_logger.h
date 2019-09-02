#ifndef __DEFAULT_LOGGER__
#define __DEFAULT_LOGGER__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "logger.h"

#define doLog(c) va_list args; \
                 va_start(args, fmt); \
                 log(c, file, line, fmt, args); \
                 va_end(args);

class DefaultLogger : public Logger 
{
public:
    const static int kMaxLogBufSize = 2048;
    bool panic_;

    void log(const char *level, const char *file, int line, const char *fmt, va_list args);

public:
    DefaultLogger() : panic_(false) 
    {
    }

    void Debugf(const char *file, int line, const char *fmt, ...) 
    {
        doLog("D");
    }
    void Infof(const char *file, int line, const char *fmt, ...) 
    {
        doLog("I");
    }
    void Warningf(const char *file, int line, const char *fmt, ...) 
    {
        doLog("W");
    }

    void Errorf(const char *file, int line, const char *fmt, ...) 
    {
        doLog("E");
    }
    void Fatalf(const char *file, int line, const char *fmt, ...) 
    {
        doLog("F");
        abort();
    }
    void Panicf(const char *file, int line, const char *fmt, ...) 
    {
        doLog("P");
    }
};

extern DefaultLogger kDefaultLogger;


#endif
