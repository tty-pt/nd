#ifndef BIOME_H
#define BIOME_H

#include "geometry.h" /* for struct obj */

#define PLANT_EXTRA 4
#define PLANT_MASK 0x3
#define PLANT_HALF (PLANT_MASK >> 1)
#define PLANT_N(pln, i) ((pln >> (i * 2)) & 3)

enum biome {
	BIOME_WATER,

	BIOME_COLD_DRY,
	BIOME_COLD,
	BIOME_COLD_WET,

	BIOME_TUNDRA,
	BIOME_TAIGA,
	BIOME_RAIN_FOREST,

	BIOME_SHRUBLAND, // / grassland / woodland
	BIOME_WOODLAND, // yellow tree
	BIOME_FOREST, // temperate

	BIOME_DESERT,
	BIOME_SAVANNAH,
	BIOME_SEASONAL_FOREST,

	BIOME_VOLCANIC,
};

typedef struct {
	const char *bg;
} biome_t;

struct plant {
	struct obj o;
	char const *pre, small, big, *post;
	coord_t tmp_min, tmp_max;
	ucoord_t rn_min, rn_max;
	unsigned char yield;
	struct obj fruit;
	unsigned y;
};

struct plant_data {
	unsigned char id[3];
	unsigned char n, max;
};

extern biome_t biomes[];
extern struct plant plants[];

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
