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
	unsigned char y, stat, lvl, lvl_v, wt, flags;
	unsigned element;
	unsigned biomes;
} SENT;

typedef struct plant_skel {
	struct print_info pi;
	char const small, big;
	int16_t tmp_min, tmp_max;
	uint16_t rn_min, rn_max;
	unsigned y;
} SPLA;

typedef struct spell_skeleton {
	unsigned element;
	unsigned char ms, ra, y, flags;
	char *name, *description;
} SSPE;

enum type {
	TYPE_ROOM,
	TYPE_THING,
	TYPE_PLANT,
	TYPE_ENTITY,
	TYPE_EQUIPMENT,
	TYPE_SEAT,
};

typedef struct object_skel {
	char const name[32];

        enum type type;

        union {
		struct {
			unsigned food;
			unsigned drink;
		} consumable;
                struct {
                        unsigned short eqw, msv;
                } equipment;
                SENT entity;
		SPLA plant;
                struct {
			const enum color bg;
                } biome;
		SSPE spell;
		unsigned raw[4];
        } sp;
} SKEL;

typedef struct {
	enum color color;
	unsigned weakness;
} element_t;

/* FIXME: not for plugins */
extern unsigned skel_hd, drop_hd, adrop_hd, element_hd;

#endif
