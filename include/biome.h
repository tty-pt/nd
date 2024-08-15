#ifndef BIOME_H
#define BIOME_H

#include "uapi/skel.h"

enum biome {
	BIOME_WATER,

	BIOME_PERMANENT_ICE,

	BIOME_TUNDRA,
	BIOME_TUNDRA2,
	BIOME_TUNDRA3,
	BIOME_TUNDRA4,

	BIOME_COLD_DESERT,
	BIOME_SHRUBLAND,
	BIOME_CONIFEROUS_FOREST,
	BIOME_BOREAL_FOREST,

	BIOME_TEMPERATE_GRASSLAND,
	BIOME_WOODLAND,
	BIOME_TEMPERATE_SEASONAL_FOREST,
	BIOME_TEMPERATE_RAINFOREST,

	BIOME_DESERT,
	BIOME_SAVANNAH,
	BIOME_TROPICAL_SEASONAL_FOREST,
	BIOME_TROPICAL_RAINFOREST,

	BIOME_VOLCANIC,

	BIOME_MAX,
};

extern unsigned biome_refs[BIOME_MAX];

#endif
