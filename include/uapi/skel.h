#ifndef UAPI_SKEL_H
#define UAPI_SKEL_H

#include <stdint.h>
#include "./azoth.h"

enum base_element {
	ELM_SPIRIT = 1,
	ELM_FIRE = 2,
	ELM_WATER = 4,
	ELM_AIR = 8,
	ELM_EARTH = 16,
	ELM_PHYSICAL = 32,
};

enum biome {
	BIOME_WATER = 0,
	BIOME_PERMANENT_ICE = 1,

	BIOME_TUNDRA = 2,
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
	BIOME_VOLCANIC = 18,
	BIOME_MAX = 19,
};

typedef struct drop {
	unsigned skel;
	unsigned char y, yield, yield_v;
} DROP;

typedef struct entity_skel {
	unsigned char y, flags;
	unsigned element;
	unsigned biomes;
} SENT;

enum type {
	TYPE_ROOM,
	TYPE_ENTITY,
};

typedef struct {
	const enum color bg;
} biome_skel_t;

typedef struct object_skel {
	char const name[32];
        enum type type;
	unsigned max_art;
	unsigned data[8];
} SKEL;

typedef struct {
	enum color color;
	unsigned weakness;
} element_t;

/* FIXME: not for plugins */
extern unsigned skel_hd, drop_hd, adrop_hd, element_hd;

#endif
