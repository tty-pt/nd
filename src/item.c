#include "item.h"

#include <stddef.h> // offsetof
#include "props.h"
#include "externs.h"
#include "kill.h"
#include "db.h"
#include "interface.h"
#include "geography.h"
#undef NDEBUG
#include "debug.h"

#define DMG_WEAPON(x) IE(x, DMG_G)
#define WTS_WEAPON(eq) phys_wts[GETEQT(eq)]

#define DEF_G(v) G(v)
#define DEF_ARMOR(x, aux) (IE(x, DEF_G) >> aux)
#define DODGE_ARMOR(def) def / 4

#define MESGPROP_MID	"_/mid"
#define SETMID(x, y)	set_property_value(x, MESGPROP_MID, y + 1)
#define GETMID(x)	(get_property_value(x, MESGPROP_MID) - 1)

enum mob_flags {
	MOB_DEFAULT,
	MOB_AGGRO,
};

enum drink_affects {
	DA_NONE,
	DA_HP,
	DA_MP,
};

struct drink {
	char *name;
	enum drink_affects da;
};

const unsigned bodypart_map[] = {
	[0 ... 254] = 0,
	['h'] = 1,
	['n'] = 2,
	['c'] = 3,
	['b'] = 4,
	['w'] = 5,
	['l'] = 6,
	['r'] = 7,
	['g'] = 8,
};

static inline int
rarity_get() {
	register int r = random();
	if (r > RAND_MAX >> 1)
		return 0; // POOR
	if (r > RAND_MAX >> 2)
		return 1; // COMMON
	if (r > RAND_MAX >> 6)
		return 2; // UNCOMMON
	if (r > RAND_MAX >> 10)
		return 3; // RARE
	if (r > RAND_MAX >> 14)
		return 4; // EPIC
	return 5; // MYTHICAL
}

int
equip_calc(struct living *p, dbref eq)
{
	register int msv = GETMSV(eq),
		 eqw = GETEQW(eq),
		 eql = EQL(eqw),
		 eqt = EQT(eqw);
	unsigned aux = 0;

	switch (eql) {
	case RHAND:
		if (GETSTAT(p->who, STR) < msv)
			return 1;
		EV(p, DMG) += DMG_WEAPON(eq);
		p->wts = WTS_WEAPON(eq);
		break;

	case HEAD:
	case PANTS:
		aux = 1;
	case CHEST:

		switch (eqt) {
		case ARMOR_LIGHT:
			if (GETSTAT(p->who, DEX) < msv)
				return 1;
			aux += 2;
			break;
		case ARMOR_MEDIUM:
			msv /= 2;
			if (GETSTAT(p->who, STR) < msv
			    || GETSTAT(p->who, DEX) < msv)
				return 1;
			aux += 1;
			break;
		case ARMOR_HEAVY:
			if (GETSTAT(p->who, STR) < msv)
				return 1;
		}
		aux = DEF_ARMOR(eq, aux);
		EV(p, DEF) += aux;
		int pd = EV(p, DODGE) - DODGE_ARMOR(aux);
		EV(p, DODGE) = pd > 0 ? pd : 0;
	}

	return 0;
}

static int
cannot_equip(struct living *liv, dbref eq)
{
	unsigned eql = GETEQL(eq);
	dbref who = liv->who;

	if (GETEQ(who, eql)
	    || equip_calc(liv, eq))
		return 1;

	SETEQ(who, eql, eq);
	DBFETCH(eq)->flags |= DARK;
	DBDIRTY(eq);

	notify_fmt(who, "You equip %s.", NAME(eq));
	return 0;
}

static inline dbref
item_add(struct living *liv, struct item i)
{
	dbref nu;

	nu = obj_add(i.o, liv->who);
	SETEQW(nu, i.eqw);
	SETMSV(nu, i.msv);
	SETRARE(nu, rarity_get());

	if (!cannot_equip(liv, nu))
		SETEQ(liv->who, i.eqw, nu);

	return nu;
}

dbref
contents_find(int descr, dbref player, dbref where, const char *name)
{
	struct match_data md;
	init_match_remote(descr, player, where, name, TYPE_THING, &md);
	match_possession(&md);
	return md.exact_match;
}

void
do_select(dbref player, const char *n_s)
{
	unsigned n = strtoul(n_s, NULL, 0);
	struct living *liv = living_get(player);
	liv->select = n;
	notify_fmt(player, "You select %u.", n);
}

void
do_equip(int descr, dbref player, char const *name)
{
	dbref eq = contents_find(descr, player, player, name);

	if (eq < 0) {
		notify(player, "You are not carrying that.");
		return;
	}

	if (cannot_equip(living_get(player), eq)) { 
		notify(player, "You can't equip that.");
		return;
	}
}

dbref
unequip(dbref who, unsigned eql)
{
	struct living *liv = living_get(who);
	dbref eq = GETEQ(who, eql);
	unsigned eqt, aux;

	if (!eq)
		return NOTHING;

	eqt = EQT(GETEQW(eq));
	aux = 0;
	assert(liv);

	switch (eql) {
	case RHAND:
		EV(liv, DMG) -= DMG_WEAPON(eq);
		liv->wts = phys_wts[liv->mob ? liv->mob->wt : GETWTS(who)];
		break;
	case PANTS:
	case HEAD:
		aux = 1;
	case CHEST:
		switch (eqt) {
		case ARMOR_LIGHT: aux += 2; break;
		case ARMOR_MEDIUM: aux += 1; break;
		}
		aux = DEF_ARMOR(eq, aux);
		EV(liv, DEF) -= aux;
		EV(liv, DODGE) += DODGE_ARMOR(aux);
	}

	SETEQ(who, eql, 0);
	DBFETCH(eq)->flags &= ~DARK;
	DBDIRTY(eq);
	return eq;
}

void
do_unequip(int descr, dbref player, char const *name)
{
	unsigned idx = bodypart_map[(int) *name];
	dbref eq;

	if ((eq = unequip(player, idx)) == NOTHING) {
		notify(player, "You don't have that equipped.");
		return;
	}

	notify_fmt(player, "You unequip %s.", NAME(eq));
}

#define F(x) (1<<(x - 1)) // "folds" value FIXME - 1

struct drop dagger_drop = {
	{
		{ "dagger", "", "", },
		EQ(RHAND, SLASH), 5
	}, 0
};

#define ARMORSET(s, type, min_stat, y) \
	struct drop s ## _helmet = { \
		{ \
			{ #s " armor helmet", "", "", }, \
			EQ(HEAD, type), min_stat, \
		}, y \
	}; \
	struct drop s ## _chest = { \
		{ \
			{ #s " armor chest", "", "", }, \
			EQ(CHEST, type), min_stat, \
		}, y \
	}; \
	struct drop s ## _pants = { \
		{ \
			{ #s " armor leggings", "", "", }, \
			EQ(PANTS, type), min_stat, \
		}, y \
	};


#define ARMORSET_LIGHT(s, min_dex, y) \
	ARMORSET(s, ARMOR_LIGHT, min_dex, y)

#define ARMORSET_MEDIUM(s, min_stat, y) \
	ARMORSET(s, ARMOR_MEDIUM, min_stat, y)

#define ARMORSET_HEAVY(s, min_str, y) \
	ARMORSET(s, ARMOR_HEAVY, min_str, y)

#define ARMORSET_LIST(s) & s ## _helmet, \
	& s ## _chest, & s ## _pants

ARMORSET_LIGHT(padded, 15, 5)
ARMORSET_MEDIUM(hide, 15, 5)
ARMORSET_HEAVY(chainmail, 15, 5)

#undef ARMORSET_LIGHT
#undef ARMORSET_MEDIUM
#undef ARMORSET_HEAVY
#undef ARMORSET

#define FIGHTER F(STR) | F(CON) | F(DEX)
#define MOB(s, de) { #s, #s, de }
#define BIRD(s, d) .o = { #s, "bird/" #s, d }, \
	.wt = PECK, .type = ELM_AIR
#define FISH(s, d) .o = { #s, "fish/" #s, d }, \
	.wt = BITE, .type = ELM_ICE

const unsigned ofs_water = 1;
const unsigned ofs_ice = 9;
const unsigned ofs_jungle = 10;
const unsigned ofs_temperate = 11;
const unsigned ofs_temperate_desert = 19;
const unsigned ofs_desert = 20;
const unsigned ofs_fire = 21;
const unsigned ofs_end = 22;

static struct mob const mobs[] = {{
/* 0 {{{ */
}, {
	FISH(goldfish, ""), .y = 4,
}, {
	FISH(salmon, ""), .y = 4,
}, {
	FISH(tuna, ""), .y = 6,
}, {
	FISH(koifish, ""), .y = 10,
}, {
	FISH(dolphin, ""), .y = 13,
}, {
	FISH(shark, ""), .y = 13, .lvl = 40, .lvl_v = 0x1f, .flags = MOB_AGGRO
}, {
	FISH(moonfish, ""), .y = 14, .flags = MOB_AGGRO
}, {
	FISH(rainbowfish, ""), .y = 14, .flags = MOB_AGGRO
/* }}} */
/* OFS_ICE {{{*/
}, {
	BIRD(icebird, ""), .y = 14, .type = ELM_ICE, .flags = MOB_AGGRO
/* }}} */
/* OFS_JUNGLE {{{ */
}, {
	.y = 1, BIRD(parrot, "")
/* }}} */
/* OFS_TEMPERATE {{{ */
}, {
	MOB(bandit, ""),
	{ &dagger_drop, ARMORSET_LIST(padded), ARMORSET_LIST(hide), ARMORSET_LIST(chainmail), NULL },
	4, FIGHTER, 20, 0x7, .flags = MOB_AGGRO
}, {
	BIRD(swallow, ""), .y = 4,
}, {
	MOB(skeleton, "Bones that almost shine with a white polish that never seems to dull."),
	{ NULL }, 2, FIGHTER, 40, 0xf, .flags = MOB_AGGRO, .type = ELM_DARK,
}, {
	MOB(vampire_spawn, "Undead that came into Being when a Vampire Slew a Mortal."),
	{ NULL }, 6, FIGHTER, 80, 0x1f,
	.type = ELM_VAMP, .flags = MOB_AGGRO,
	.o.name = "vampire spawn",
}, {
	BIRD(woodpecker, ""), .y = 2,
}, {
	BIRD(sparrow, ""), .y = 3,
}, {
	BIRD(owl, ""), .y = 7, .type = ELM_DARK,
}, {
	BIRD(eagle, ""), .y = 7,
/* }}} */
/* OFS_TEMPERATE_DESERT {{{ */
}, {
	BIRD(starling, ""), .y = 8,
/* }}} */
/* OFS_DESERT {{{ */
}, {
	BIRD(vulture, ""), .y = 7,
/* }}} */
/* OFS_FIRE {{{ */
}, {
	BIRD(firebird, ""), .y = 14, .type = ELM_FIRE, .flags = MOB_AGGRO,
/* }}} */
}};

#undef FISH
#undef BIRD
#undef MOB
#undef FIGHTER
#undef F

static inline void
mob_add_stats(struct mob const *mob, dbref nu)
{
	unsigned char stat = mob->stat;
	int lvl = mob->lvl, spend, i, sp, v = mob->lvl_v ? mob->lvl_v : 0xf;

	lvl += random() & v;

	if (!stat)
		stat = 0x1f;

	spend = 1 + lvl;
	for (i = 0; i < STAT_MAX; i++)
		if (stat & (1<<i)) {
			sp = random() % spend;
			SETSTAT(nu, i + 1, sp);
		}

	SETLVL(nu, lvl);
}

static inline void
mob_inventory(struct living *liv, struct drop **drop)
{
	for (; *drop; drop++)
		if (random() < (RAND_MAX >> (*drop)->y))
			item_add(liv, (*drop)->i);
}

static inline int
bird_is(struct mob const *mob)
{
	return mob->wt == PECK;
}

static inline dbref
mob_add(unsigned mid, dbref where, struct bio *b) {
	struct mob const *mob = &mobs[mid];
	struct drop *drop[8];
	struct living *liv;
	dbref nu;

	if ((bird_is(mob) && !(b->pd.n))
	    || (!NIGHT_IS && (mob->type == ELM_DARK || mob->type == ELM_VAMP))
	    || random() >= (RAND_MAX >> mob->y))
		return NOTHING;

	nu = obj_add(mob->o, where);
	memset(drop, 0, sizeof(drop));

	SETMID(nu, mid);
	SETAGGRO(nu, mob->flags & MOB_AGGRO);
	mob_add_stats(mob, nu);
	liv = living_put(nu);

	debug("%d %s loc %d %s MID %d", nu, NAME(nu), where, NAME(where), mid);

	mob_inventory(liv, (struct drop **) mob->drop);

	if (bird_is(mob)) {
		struct drop feather = { {{ "%s's feather", "", "" }}, 1 };
		char name[32];
		snprintf(name, sizeof(name), feather.i.o.name, NAME(nu));
		feather.i.o.name = name;
		drop[0] = &feather;
	}
	/* else if (fish_is(mob)) { */
		/* struct drop scale = { {{ "%s's scale", "", "" }}, 1 }; */
		/* char name[32]; */
		/* snprintf(name, sizeof(name), scale.i.o.name, NAME(nu)); */
		/* feather.i.o.name = name; */
		/* drop[0] = &feather; */
	/* } */

	mob_inventory(liv, drop);

	return nu;
}

void
mobs_add(struct bio *b, dbref w) {
	unsigned mid,
		 n = 0;
	int o = 0;
	CBUG(w <= 0);

	// TODO more consistent logic that doesn't need
	// me to mess around with this index stuff

	switch (b->bio_idx) {
	case BIOME_WATER:
		o = ofs_water;
		n = ofs_ice;
		break;

	case BIOME_COLD_DRY:
	case BIOME_COLD:
	case BIOME_COLD_WET:
	case BIOME_TUNDRA:
		o = ofs_ice;
		n = ofs_jungle - o;
		break;

	case BIOME_RAIN_FOREST:
		o = ofs_jungle;
		n = ofs_temperate_desert - o;
		break;

	case BIOME_TAIGA:
	case BIOME_SHRUBLAND: // / grassland / woodland
	case BIOME_WOODLAND: // yellow tree
	case BIOME_FOREST: // temperate
		o = ofs_temperate;
		n = ofs_desert - ofs_jungle;
		break;

	case BIOME_DESERT:
	case BIOME_SAVANNAH:
		o = ofs_desert;
		n = ofs_fire - o;
		break;

	case BIOME_SEASONAL_FOREST:
	case BIOME_VOLCANIC:
		o = ofs_fire;
		n = ofs_end - o;
	}

	for (mid = o; mid < o + n; mid++)
		mob_add(mid, w, b);
}

struct obj const *
mob_obj_random()
{
	int idx = random() % ofs_end;
	if (idx == 0 || idx == 5 || idx == 8 || idx == 9)
		return NULL;
	return &mobs[idx].o;
}

void
mobs_aggro(int descr, dbref player)
{
        struct living *me;
	dbref tmp;
	int klock = 0;

        CBUG(GETLID(player) < 0);
        me = living_get(player);
        if (me->who != player)
                me = living_put(player);

	DOLIST(tmp, DBFETCH(getloc(player))->contents) {
		int lid = GETLID(tmp);
		if (lid >= 0 && GETAGGRO(tmp)) {
			struct living *liv = living_get(tmp);
			liv->target = me;
			klock++;
		}
	}

	SETKLOCK(player, GETKLOCK(player) + klock);
}

struct mob const *
mob_get(dbref who)
{
	int mid = GETMID(who);
	if (mid < 0)
		return NULL;
	else
		return &mobs[mid];
}

static struct drink *
drink_get(int n)
{
	static struct drink drinks[] = {{
		"water", DA_NONE,
	}, {
		"milk", DA_HP,
	}, {
		"ayuhasca", DA_MP
	}};

	assert(n < sizeof(drinks) / sizeof(struct drink));
	return &drinks[n];
}

static inline struct drink *
source_drink_get(dbref what)
{
	return drink_get(GETDRINK(what));
}

static inline int
drink(dbref who, dbref source)
{
	static const size_t affects_ofs[] = {
		0,
		offsetof(struct living, hp),
		offsetof(struct living, mp),
	};

	struct living *liv = living_get(who);
	struct drink *drink = source_drink_get(source);
	unsigned n = GETCONSUN(source);
	unsigned const value = 2;
	int aux;

	assert(liv);

	if (!n) {
		notify_fmt(who, "%s is empty.", NAME(source));
		return 1;
	}

	* (unsigned *) (((char *) liv) + affects_ofs[drink->da]) += 1 << value;
	aux = liv->thirst - (1 << (16 - value));
	if (aux < 0)
		aux = 0;
	liv->thirst = aux;

	SETCONSUN(source, n - 1);
	notify_wts(who, "drink", "drinks", " %s", drink->name);
	return 0;
}

void
do_drink(int descr, dbref player, const char *name)
{
	dbref vial;

	if (!*name) {
		notify(player, "Drink what?");
		return;
	}

	vial = contents_find(descr, player, player, name);

	if (vial == NOTHING) {
		notify(player, "Drink what?");
		return;
	}

	drink(player, vial);
	/* if (drink(player, vial)) { */
	/* 	notify(player, "You can't drink from that."); */
	/* 	return; */
	/* } */
}

void
do_fill(int descr, dbref player, const char *vial_s, const char *source_s)
{
	dbref vial = contents_find(descr, player, player, vial_s);
	unsigned m, n;

	if (vial == NOTHING) {
		notify(player, "Fill what?");
		return;
	}

	m = GETCONSUM(vial);
	if (!m) {
		notify_fmt(player, "You can't fill %s.", NAME(vial));
		return;
	}

	n = GETCONSUN(vial);
	if (n) {
		notify_fmt(player, "%s is not empty.", NAME(vial));
		return;
	}

	dbref source = contents_find(descr, player, getloc(player), source_s);

	if (source < 0) {
		notify(player, "Invalid source.");
		return;
	}

	int dr = GETDRINK(source);

	if (dr < 0) {
		notify(player, "Invalid source.");
		return;
	}

	SETCONSUN(vial, m);
	SETDRINK(vial, dr);

	notify_wts(player, "fill", "fills", " %s with %s from %s",
		   NAME(vial), drink_get(dr)->name, NAME(source));
}

void
do_eat(int descr, dbref player, const char *what)
{
	struct living *p = living_get(player);
	dbref item = contents_find(descr, player, player, what);
	int food;
	unsigned stack;

	if (item < 0
	    || (food = GETFOOD(item)) < 0) {
		notify(player, "You can't eat that.");
		return;
	}

	stack = GETSTACK(item);
	// TODO food should have some water content
	food = p->hunger - (1 << (16 - food));
	if (food < 0)
		food = 0;
	p->hunger = food;
	notify_wts(player, "eat", "eats", " %s", NAME(item));
	if (!stack) {
		recycle(descr, player, item);
		return;
	}
	SETSTACK(item, stack - 1);
}
