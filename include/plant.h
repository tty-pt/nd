#ifndef PLANT_H
#define PLANT_H

#include "uapi/object.h"
#include "st.h"

#define PLANT_EXTRA 4
#define PLANT_MASK 0x3
#define PLANT_HALF (PLANT_MASK >> 1)
#define PLANT_N(pln, i) ((pln >> (i * 2)) & 3)

struct plant_data {
	unsigned char id[3];
	unsigned char n, max;
};

void
plants_noise(struct plant_data *, unsigned ty,
		coord_t tmp, ucoord_t rn, unsigned n);

void plants_shuffle(struct plant_data *, morton_t v);

#endif
