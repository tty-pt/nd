// TODO blocking, parry

#ifndef ITEM_H
#define ITEM_H

#include "db.h"

#define RARE_MAX 6

#define G(x) xsqrtx(x)
#define MSRA(ms, ra, G) G(ms) * (ra + 1) / RARE_MAX

#define IE(x, G) MSRA(GETMSV(x), GETRARE(x), G)
#define HS(sp) MSRA(sp->ms, sp->ra, SPELL_G)

#define DMG_G(v) G(v)
#define DMG_BASE(p) DMG_G(GETSTAT(p, STR))
#define DMG_WEAPON(x) IE(x, DMG_G)
#define WTS_WEAPON(eq) phys_wts[GETEQT(eq)]

#define HP_G(v) 10 * G(v)
#define HP_MAX(p) HP_G(GETSTAT(p, CON))

#define DEF_G(v) G(v)
#define DEF_ARMOR(x, aux) (IE(x, DEF_G) >> aux)

/* TODO some of this kind of macros are in kill.c */

#define DODGE_G(v) G(v)
#define DODGE_BASE(p) DODGE_G(GETSTAT(p, DEX))
#define DODGE_ARMOR(def) def / 4

#define MP_G(v) HP_G(v)
#define MP_MAX(p) MP_G(GETSTAT(p, WIZ))

#define SPELL_G(v) G(v)
#define SPELL_DMG(p, sp) SPELL_G(GETSTAT(p, INT)) + HS(sp)
#define SPELL_COST(dmg, y, no_bdmg) (no_bdmg ? 0 : dmg) + dmg / (1 << y)

#define BUF_DURATION(ra) 20 * (RARE_MAX - ra) / RARE_MAX
#define BUF_DMG(sp_dmg, duration) ((long) 2 * sp_dmg) / duration

#define EV(liv, w) liv->e[AF_ ## w].value
#define EM(liv, w) liv->e[AF_ ## w].mask

#define BUF_TYPE_MASK 0xf
#define BUF_TYPE(sp) (sp->flags & BUF_TYPE_MASK)

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

enum biome_type {
	BIOME_WATER,

	BIOME_COLD_DRY,
	BIOME_COLD,
	BIOME_COLD_WET,

	BIOME_TUNDRA,
	BIOME_TAIGA,
	BIOME_RAIN_FOREST,

	BIOME_SHRUBLAND, // / grassland / woodland
	BIOME_WOODLAND, // yellow tree
	BIOME_FOREST, // temperate

	BIOME_DESERT,
	BIOME_SAVANNAH,
	BIOME_SEASONAL_FOREST,

	BIOME_VOLCANIC,
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

struct obj {
	char const *name;
	char const *art;
	char const *description;
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

dbref obj_add(struct obj o, dbref where);
dbref contents_find(int descr, dbref player, dbref what, const char *name);

int equip_calc(struct living *p, dbref eq);
dbref unequip(dbref player, unsigned eql);
void mobs_add(enum biome_type b, dbref where, dbref tree);
struct mob const *mob_random();
void mobs_aggro(int descr, dbref player);
struct mob const *mob_get(dbref who);

#endif
