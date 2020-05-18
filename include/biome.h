#ifndef BIOME_H
#define BIOME_H

#include "geometry.h" /* for struct obj */

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

extern biome_t biomes[];

#endif
