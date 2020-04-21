#ifndef LOG_H
#define LOG_H

#ifdef LOG_TO_STDERR
#include <stdio.h>
#define LOGLOG(format, ...) \
{ \
fprintf(stderr ,format ,##__VA_ARGS__); \
    fprintf(stderr ,"\n"); \
    }
#else
int jklog(const char* ,...);
#define LOGLOG(format, ...) \
{ \
    jklog(format ,##__VA_ARGS__); \
    }
#endif
void log_init();
#endif // LOG_H
