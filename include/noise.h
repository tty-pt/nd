#ifndef ND_NOISE_H
#define ND_NOISE_H

#include "noise_decl.h"
#include "biome.h"
#include "plant.h"

struct bio {
	coord_t tmp;
	ucoord_t rn;
	noise_t ty;
	struct plant_data pd;
	enum biome bio_idx;
};

#endif
