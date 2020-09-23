#ifndef HASH_H
#define HASH_H

#include "xxhash.h"

static inline unsigned
uhash(const char *p, size_t l, int seed)
{
	return XXH32(p, l, seed);
}

static inline unsigned
hash_idx(const char *str, unsigned size)
{
	unsigned h = uhash(str, strlen(str), 88);
	return h % size;
}

#endif
