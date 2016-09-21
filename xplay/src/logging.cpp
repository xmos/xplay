#include <mutex>
#include "logging.h"
#include "stdio.h"
#include "stdarg.h"

std::mutex out_mutex;

void log(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    out_mutex.lock();
    vprintf(format, args);
    fflush(stdout);
    out_mutex.unlock();

    va_end(args);
}

void report_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    
    out_mutex.lock();
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, format, args);
    fflush(stderr);
    out_mutex.unlock();
    va_end(args);
}
