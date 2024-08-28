#ifndef PLANT_H
#define PLANT_H

#include "object.h"
#include "spacetime.h"

#define PLANT_EXTRA 4
#define PLANT_MASK 0x3
#define PLANT_HALF (PLANT_MASK >> 1)
#define PLANT_N(pln, i) ((pln >> (i * 2)) & 3)

struct plant_data {
	unsigned char id[3];
	unsigned char n, max;
};

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

enum plant {
	PLANT_PINUS_SILVESTRIS,
	PLANT_PSEUDOTSUGA_MENZIESII,
	PLANT_BETULA_PENDULA,
	PLANT_LINUM_USITATISSIMUM,
	PLANT_BETULA_PUBESCENS,
	PLANT_ABIES_ALBA,
	PLANT_ARTHROCEREUS_RONDONIANUS,
	PLANT_ACACIA_SENEGAL,
	PLANT_DAUCUS_CAROTA,
	PLANT_SOLANUM_LYCOPERSICUM,
	PLANT_MAX,
};

extern unsigned plant_refs[PLANT_MAX];

void plants_init();

#endif
