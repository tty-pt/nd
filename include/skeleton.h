#ifndef SKELETON_H
#define SKELETON_H

#include "stdint.h"

typedef int16_t coord_t;
typedef uint16_t ucoord_t;

enum element {
	ELM_PHYSICAL,
	ELM_FIRE,
	ELM_ICE,
	ELM_AIR,
	ELM_EARTH,
	ELM_SPIRIT,
	ELM_VAMP,
	ELM_DARK,
};

struct drop {
	struct object_skeleton *i;
	unsigned char y, yield, yield_v;
};

typedef struct entity_skeleton {
	struct drop *drop[32];
	unsigned char y, stat, lvl, lvl_v, wt, flags;
	enum element type;
	unsigned biomes;
} SENT;

struct plant_skeleton {
	char const *pre, small, big, *post;
	coord_t tmp_min, tmp_max;
	ucoord_t rn_min, rn_max;
	struct drop *drop[32];
	unsigned y;
};

enum object_skeleton_type {
        S_TYPE_OTHER,
        S_TYPE_CONSUMABLE,
        S_TYPE_EQUIPMENT,
        S_TYPE_ENTITY,
	S_TYPE_PLANT,
	S_TYPE_BIOME,
};

typedef struct object_skeleton {
	char const *name;
	char const *art;
	char const *description;
        char const *avatar;

        enum object_skeleton_type type;

        union {
		struct {
			unsigned food;
			unsigned drink;
		} consumable;
                struct {
                        unsigned short eqw, msv;
                } equipment;
                struct entity_skeleton entity;
		struct plant_skeleton plant;
                struct {
                        const char *bg;
                } biome;
        } sp;
} SKEL;

struct spell_skeleton {
	struct object_skeleton o;
	enum element element;
	unsigned char ms, ra, y, flags;
};

#endif
