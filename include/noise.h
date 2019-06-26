#ifndef NOISE_H
#define NOISE_H

#include "geometry.h"
#include "xxhash.h"
#include "item.h"

// TODO make these vars?
#define VIEW_AROUND 3
#define VIEW_SIZE ((VIEW_AROUND<<1) + 1)
#define VIEW_M VIEW_SIZE * VIEW_SIZE

typedef XXH32_hash_t noise_t;
typedef long snoise_t;

struct bio {
	coord_t tmp;
	ucoord_t rn;
	noise_t he;
	noise_t cl;
	noise_t tm;
	unsigned tree_n;
	enum biome_type bio_idx;
};

struct biome {
	struct obj tree;
	char const *tree_wide, *tree_side, *bg;
};

extern struct biome const biomes[];

struct bio noise_point(morton_t p);
void noise_view(struct bio to[VIEW_M], point_t s, ucoord_t obits);

#endif
