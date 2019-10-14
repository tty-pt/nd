#ifndef NOISE_H
#define NOISE_H

#include "geometry.h"
#include "xxhash.h"

// TODO make these vars?
#define VIEW_AROUND 3
#define VIEW_SIZE ((VIEW_AROUND<<1) + 1)
#define VIEW_M VIEW_SIZE * VIEW_SIZE
#define EXTRA_TREE 4
#define TREE_N_MASK 0x3
#define TREE_HALF (TREE_N_MASK >> 1)
#define TREE_N(pln, i) ((pln >> (i * 2)) & 3)
#define B_TREE_N(b, i) TREE_N(b->pln, i)

typedef XXH32_hash_t noise_t;
typedef long snoise_t;

struct bio {
	coord_t tmp;
	ucoord_t rn;
	noise_t ty;
	unsigned char plid[3], mplid;
	unsigned char pln;
	enum biome_type bio_idx;
};

struct plant {
	struct obj o;
	char const *pre, small, big, *post;
	coord_t tmp_min, tmp_max;
	ucoord_t rn_min, rn_max;
	unsigned char yield;
	struct obj fruit;
	unsigned y;
};

extern struct plant plants[];

struct bio * noise_point(morton_t p);
void noise_view(struct bio to[VIEW_M], point_t pos, ucoord_t obits);
unsigned char noise_rplants(unsigned char plid[EXTRA_TREE], unsigned char *pln, struct bio *b);

#endif
