#ifdef NDEBUG
#define debug(...) do {} while (0)
#define bassert(c) do {} while (0)
#else
#include <stdio.h>
#include <signal.h>
#define debug(...) fprintf(stderr, __VA_ARGS__)
#define bassert(c) if (!(c)) raise(SIGINT)
#endif
