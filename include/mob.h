#ifndef MOB_H
#define MOB_H

#include "biome.h"
#include "spell.h"
#include "item.h"

/* FIXME use ref as index */
#define ENTITY_SKELETON(mob_id) (&entity_skeleton_map[mob_id])

#define BODYPART_ID(_c) ch_bodypart_map[(int) _c]
/* #define BODYPART(_c) bodypart_map[BODYPART_ID(_c)] */

typedef struct {
	char *name;
} bodypart_t;

/* FIXME also refered to by eql */
enum bodypart {
	BP_NULL,
	BP_HEAD,
	BP_NECK,
	BP_CHEST,
	BP_BACK,
	BP_WEAPON,
	BP_LFINGER,
	BP_RFINGER,
	BP_LEGS,
};

extern bodypart_t bodypart_map[];
extern enum bodypart ch_bodypart_map[];

enum mob_ofs {
	MOFS_WATER = 1,
	MOFS_ICE = 9,
	MOFS_JUNGLE = 10,
	MOFS_TEMPERATE = 11,
	MOFS_TEMPERATE_DESERT = 19,
	MOFS_DESERT = 20,
	MOFS_FIRE = 21,
	MOFS_END = 22,
};

/* const unsigned ofs_water = 1; */
enum mob_type {
	MOB_HUMAN = 0,
	MOB_GOLDFISH,
	MOB_SALMON,
	MOB_TUNA,
	MOB_KOIFISH,
	MOB_DOLPHIN,
	MOB_SHARK,
	MOB_MOONFISH,
	MOB_RAINBOWFISH,
	MOB_ICEBIRD = MOFS_ICE,
	MOB_PARROT = MOFS_JUNGLE,
	MOB_BANDIT = MOFS_TEMPERATE,
	MOB_SWALLOW,
	/* MOB_SKELETON_, */
	MOB_VAMPIRE_SPAWN,
	MOB_WOODPECKER,
	MOB_SPARROW,
	/* MOB_OWL, */
	/* MOB_EAGLE, */
	/* MOB_STARLING = MOFS_TEMPERATE_DESERT, */
	/* MOB_VULTURE = MOFS_DESERT, */
	/* MOB_FIREBIRD = MOFS_FIRE, */
	MOB_MAX,
};

void entities_add(OBJ *where, enum biome, long long pdn);
ENT *birth(OBJ *player);
void mobs_aggro(OBJ *player);
struct object_skeleton const *mob_obj_random();
void entity_update(OBJ *player);

#endif
