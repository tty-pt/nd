#ifndef PLANT_H
#define PLANT_H

#include "object.h"
#include "spacetime.h"

#define PLANT_EXTRA 4
#define PLANT_MASK 0x3
#define PLANT_HALF (PLANT_MASK >> 1)
#define PLANT_N(pln, i) ((pln >> (i * 2)) & 3)
#define PLANT_MAX (sizeof(plant_skeleton_map) / sizeof(struct object_skeleton))

#define PLANT_SKELETON(plid) &plant_skeleton_map[plid]

struct plant_data {
	unsigned char id[3];
	unsigned char n, max;
};

extern struct object_skeleton plant_skeleton_map[];

void
plants_noise(struct plant_data *,
		unsigned ty,
		coord_t tmp, ucoord_t rn,
		unsigned n);

void
plants_shuffle(struct plant_data *,
		morton_t v);

void
plants_add(dbref where_ref, void *bio, pos_t pos);

#endif
