#ifndef HASH_H
#define HASH_H

#include "xxhash.h"

static inline unsigned
uhash(const char *p, size_t l, int seed)
{
	return XXH32(p, l, seed);
}

#endif
