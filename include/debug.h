#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
/* #define DEBUG(...) fprintf(stderr, __VA_ARGS__) */
/* #define debug(...) debug # __FILE__(__VA_ARGS__) */
#define debug(...) fprintf(stderr, __VA_ARGS__)
/* #define debug(...) do {} while (0) */

#endif /* DEBUG_H */

