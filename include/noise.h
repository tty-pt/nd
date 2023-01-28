#ifndef ND_NOISE_H
#define ND_NOISE_H

#include "nd/noise.h"
#include "biome.h"
#include "plant.h"
#include "hash.h"

struct bio {
	coord_t tmp;
	ucoord_t rn;
	noise_t ty;
	struct plant_data pd;
	enum biome bio_idx;
};

struct bio * noise_point(pos_t p);
void noise_view(struct bio to[VIEW_M], pos_t pos);

#endif
