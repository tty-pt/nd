#ifndef UAPI_WTS_H
#define UAPI_WTS_H

#include "./object.h"

typedef char *wts_plural_t(char *singular);
wts_plural_t wts_plural;

static inline char *wts_cond(char *singular, int number) {
	return number == 1 || number == -1 ? singular : wts_plural(singular);
}

#endif
