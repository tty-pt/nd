#ifndef MOB_H
#define MOB_H

#include "biome.h"
#include "spell.h"
#include "item.h"

/* FIXME move to db.h? */
#define MESGPROP_MID	"_/mid"
#define SETMID(x, y)	set_property_value(x, MESGPROP_MID, y + 1)
#define GETMID(x)	(get_property_value(x, MESGPROP_MID) - 1)


/* FIXME use ref as index */
#define MOB(who) (db[who].mob)

#define MOB_SKELETON(mob_id) (&mob_skeleton_map[mob_id])

#define MOB_EV(mob, w) mob->e[AF_ ## w].value
#define MOB_EM(mob, w) mob->e[AF_ ## w].mask

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

enum mob_flags {
	MF_DEFAULT,
	MF_NULL,
	MF_AGGRO,
	MF_SITTING,
};

/* instance of mob */
struct mob {
	struct debuf debufs[8];
	struct spell spells[8];
	effect_t e[7];
	dbref target;
	struct wts wts;
	int descr;
	unsigned respawn_in, flags; // TODO merge these two
	unsigned short hp, mp, hunger, thirst;
	unsigned char debuf_mask, combo, select, klock;
};

void mobs_add(dbref where, enum biome, long long pdn);
struct mob *mob_put(dbref where);
void mobs_aggro(dbref player);
struct object_skeleton const *mob_obj_random();
void mob_update(dbref who, long long unsigned tick);
void mob_add_stats(struct object_skeleton *mob, dbref nu);

#endif
