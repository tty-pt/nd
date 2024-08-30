#ifndef UAPI_WTS_H
#define UAPI_WTS_H

#include "./object.h"

enum wt {
	WT_PUNCH,
	WT_PECK,
	WT_SLASH,
	WT_BITE,
	WT_STRIKE,
	WT_SLAM,
	WT_KICK,
	WT_THRUST,
	WT_STAB,
	WT_HIT,
	WT_CRUSH,
	WT_CHOP,
	WT_SMASH,
	WT_THROW,
	WT_POUND,
	WT_BLUDGEON,
	WT_LASH,
	WT_RIP,
	WT_TEAR,
	WT_BASH,
	WT_TRAMPLE,
	WT_HEADBUTT,
	WT_BACKHAND,
	WT_UPPERCUT,
	WT_SHOVE,
	WT_KNEE,
	WT_ELBOW,
	WT_FLAIL,
	WT_GNAW,
	WT_PIERCE,
	WT_WHIP,
	WT_SLAP,
	WT_CLAW,
	WT_MAUL,
	WT_MACE,
	WT_BATTER,
	WT_HAMMER,
	WT_SPIN,
	WT_TACKLE,
	WT_PIN,
	WT_GRAPPLE,
	WT_HOOK,
	WT_JAB,
	WT_SLICE,
	WT_PUMMEL,
	WT_FLING,
	WT_STOMP,
	WT_BLAST,
	WT_ZAP,
	WT_SIZZLE,
	WT_BURN,
	WT_SCALD,
	WT_PEEL,
	WT_BRUISE,
	WT_STING,
};

extern char *wts_map[];

typedef char *wts_plural_t(char *singular);
wts_plural_t wts_plural;

static inline char *wts_cond(char *singular, int number) {
	return number == 1 || number == -1 ? singular : wts_plural(singular);
}
/* char ** wts_get(OBJ *player); */

#endif
