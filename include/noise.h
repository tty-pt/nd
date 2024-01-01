#ifndef ND_NOISE_H
#define ND_NOISE_H

#include "nd/noise.h"
#include "biome.h"
#include "plant.h"

struct bio {
	coord_t tmp;
	ucoord_t rn;
	noise_t ty;
	struct plant_data pd;
	enum biome bio_idx;
};

struct bio * noise_point(pos_t p);
unsigned _bio_idx(coord_t tmp_f, coord_t tmp_c, ucoord_t rain_f, ucoord_t rain_c, coord_t tmp, ucoord_t rain);
void noise_view(struct bio to[VIEW_M], pos_t pos);

#endif
