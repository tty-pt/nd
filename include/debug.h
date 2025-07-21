#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef NDEBUG
/* #define BREAK do {} while (0) */
#define BUG(...) abort()
#define CBUG(c) if (c) abort()
#else
#include <signal.h>
#include <syslog.h>
#define BUG(fmt, ...) { syslog(LOG_ERR, "BUG! %s:%s:%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); raise(SIGINT); }
#define CBUG(c) if (c) { syslog(LOG_ERR, "CBUG! " #c " %s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__); raise(SIGINT); } 
/* #define BREAK raise(SIGINT) */
#endif

#endif
