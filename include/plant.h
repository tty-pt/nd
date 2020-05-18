#ifndef PLANT_H
#define PLANT_H

#include "geometry.h"

#define PLANT_EXTRA 4
#define PLANT_MASK 0x3
#define PLANT_HALF (PLANT_MASK >> 1)
#define PLANT_N(pln, i) ((pln >> (i * 2)) & 3)
#define PLANT_MAX (sizeof(plant_map) / sizeof(plant_t))

#define PLANT(plid) &plant_map[plid]

typedef struct {
	struct obj o;
	char const *pre, small, big, *post;
	coord_t tmp_min, tmp_max;
	ucoord_t rn_min, rn_max;
	unsigned char yield;
	struct obj fruit;
	unsigned y;
} plant_t;

struct plant_data {
	unsigned char id[3];
	unsigned char n, max;
};

extern plant_t plant_map[];

void
plants_noise(struct plant_data *,
		morton_t ty,
		coord_t tmp, ucoord_t rn,
		unsigned n);

void
plants_shuffle(struct plant_data *,
		morton_t v);

void
plants_add(int descr, ref_t player, ref_t where,
		struct plant_data *pd,
		morton_t ty, coord_t tmp,
		ucoord_t rn);

#endif
