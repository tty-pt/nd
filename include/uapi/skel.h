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

enum object_skeleton_type {
        STYPE_OTHER,
        STYPE_CONSUMABLE,
        STYPE_EQUIPMENT,
        STYPE_ENTITY,
	STYPE_PLANT,
	STYPE_BIOME,
	STYPE_MINERAL,
	STYPE_SPELL,
};

enum biome {
	BIOME_WATER = 0,
	BIOME_PERMANENT_ICE = 1,
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

typedef struct object_skel {
	char const name[32];

        enum object_skeleton_type type;

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
                struct {
                        short unsigned unused;
                } mineral;
		SSPE spell;
        } sp;
} SKEL;

typedef struct {
	enum color color;
	unsigned weakness;
} element_t;

/* FIXME: not for plugins */
extern unsigned skel_hd, drop_hd, adrop_hd, element_hd;

#endif
