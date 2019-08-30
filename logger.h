#ifndef __LOGGER__H
#define __LOGGER__H

class Logger
{
public:
    virtual void Debugf(const char *file, int line, const char *fmt, ...) = 0;
    virtual void Infof(const char *file, int line, const char *fmt, ...) = 0;
    virtual void Warningf(const char *file, int line, const char *fmt, ...) = 0;
    virtual void Errorf(const char *file, int line, const char *fmt, ...) = 0;
    virtual void Fatalf(const char *file, int line, const char *fmt, ...) = 0;
    virtual void Panicf(const char *file, int line, const char *fmt, ...) = 0;
};

#endif
