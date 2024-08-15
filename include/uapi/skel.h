#ifndef UAPI_SKEL_H
#define UAPI_SKEL_H

#include "qhash.h"

enum element {
	ELM_PHYSICAL,
	ELM_FIRE,
	ELM_ICE,
	ELM_AIR,
	ELM_EARTH,
	ELM_SPIRIT,
	ELM_VAMP,
	ELM_DARK,
	ELM_MAX,
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

typedef struct drop {
	unsigned skel;
	unsigned char y, yield, yield_v;
} DROP;

typedef struct entity_skel {
	unsigned char y, stat, lvl, lvl_v, wt, flags;
	enum element type;
	unsigned biomes;
} SENT;

typedef struct plant_skel {
	char const *pre, small, big, *post;
	int16_t tmp_min, tmp_max;
	uint16_t rn_min, rn_max;
	unsigned y;
} SPLA;

typedef struct spell_skeleton {
       enum element element;
       unsigned char ms, ra, y, flags;
       char *name, *description;
} SSPE;

typedef struct object_skel {
	char const *name;
	char const *description;

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
                        const char *bg;
                } biome;
                struct {
                        short unsigned unused;
                } mineral;
		SSPE spell;
        } sp;
} SKEL;

typedef struct {
	char *color;
	enum element weakness;
} element_t;

LHASH_DECL(skel, SKEL);
LHASH_DECL(drop, DROP);
LHASH_ASSOC_DECL(adrop, skel, drop);

LHASH_DECL(element, element_t);

/* is like:
unsigned skel_new(SKEL *skel);
SKEL skel_get(unsigned ref);
void skel_set(unsigned ref, SKEL *skel);

unsigned drop_new(DROP *drop);
DROP drop_get(unsigned ref);
void drop_set(unsigned ref, DROP *drop);

void drop_associate(unsigned skel_ref, unsigned drop_ref);
struct hash_cursor drop_iter(unsigned skel_ref);
unsigned drop_next(DROP *drop, struct hash_cursor *c);
unsigned drop_vdel(unsigned skel_ref, unsigned drop_ref);
 */

#endif
