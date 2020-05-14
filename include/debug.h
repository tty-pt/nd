#ifdef NDEBUG
#define debug(...) do {} while (0)
#define BREAK do {} while (0)
#define BUG(...) abort()
#define CBUG(c) if (c) abort()
#else
#include <stdio.h>
#include <signal.h>
#define debug(fmt, ...) fprintf(stderr, "D %s:%s:%d " fmt "\n", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define BUG(fmt, ...) { fprintf(stderr, "BUG! %s:%s:%d " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); raise(SIGINT); }
#define CBUG(c) if (c) { fprintf(stderr, "CBUG! " #c " %s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__); raise(SIGINT); } 
#define BREAK raise(SIGINT)
#endif
