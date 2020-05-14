// TODO blocking, parry

#ifndef ITEM_H
#define ITEM_H

#include "geometry.h"
#include "db.h"

enum elm_type {
	ELM_PHYSICAL,
	ELM_FIRE,
	ELM_ICE,
	ELM_AIR,
	ELM_EARTH,
	ELM_SPIRIT,
	ELM_VAMP,
	ELM_DARK,
};

enum af {
	// these are changed by bufs
	AF_HP,
	AF_MOV,
	AF_MDMG,
	AF_MDEF,
	AF_DODGE,

	// these aren't.
	AF_DMG,
	AF_DEF,

	// these are flags, not types of buf
	AF_NEG = 0x10,
	AF_BUF = 0x20,
};

struct item {
	struct obj o;
	unsigned short eqw, msv;
};

struct drop {
	struct item i;
	unsigned y;
};

struct mob {
	struct obj o;
	struct drop *drop[32];
	// y max 63 (6 bit) 
	unsigned char y, stat, lvl, lvl_v, wt, flags;
	enum elm_type type;
};

struct wts {
	const char *a, *b;
};

struct _spell {
	struct obj o;
	enum elm_type elm_type;
	unsigned char ms, ra, y, flags;
};

struct debuf { // one for each type of spell
	struct _spell *_sp;
	unsigned duration;
	short val;
};

struct spell {
	struct _spell *_sp;
	unsigned cost; 
	unsigned short val;
};

struct effective {
	short value;
	unsigned char mask;
};

struct living {
	struct debuf debufs[8];
	struct spell spells[8];
	struct effective e[7];
	struct mob const *mob;
	struct living *target;
	struct wts wts;
	dbref who;
	int descr;
	unsigned respawn_in, flags; // TODO merge these two
	unsigned short hp, mp, hunger, thirst;
	unsigned char debuf_mask, combo, select;
};

extern struct wts phys_wts[];

static inline unsigned
xsqrtx(unsigned x)
{
	// y * x ^ 3/2
	return x * sqrt(x);
}

#include "noise.h"

dbref obj_add(struct obj o, dbref where);
dbref contents_find(int descr, dbref player, dbref what, const char *name);
int equip_calc(struct living *p, dbref eq);
dbref unequip(dbref player, unsigned eql);
void mobs_add(struct bio *b, dbref where);
struct obj const *mob_obj_random();
void mobs_aggro(int descr, dbref player);
struct mob const *mob_get(dbref who);

#endif
