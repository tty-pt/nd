#ifndef MOB_H
#define MOB_H

#include "biome.h"
#include "spell.h"

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

enum mob_type {
	MOB_HUMAN,
	MOB_GOLDFISH,
	MOB_SALMON,
	MOB_TUNA,
	MOB_KOIFISH,
	MOB_DOLPHIN,
	MOB_SHARK,
	MOB_MOONFISH,
	MOB_RAINBOWFISH,
	MOB_ICEBIRD,
	MOB_PARROT,
	MOB_BANDIT,
	MOB_SWALLOW,
	/* MOB_SKELETON_, */
	MOB_VAMPIRE_SPAWN,
	MOB_WOODPECKER,
	MOB_SPARROW,
	MOB_OWL,
	/* MOB_EAGLE, */
	/* MOB_STARLING, */
	/* MOB_VULTURE, */
	/* MOB_FIREBIRD, */
	MOB_MAX,
};

extern struct object_skeleton entity_skeleton_map[];

#endif
