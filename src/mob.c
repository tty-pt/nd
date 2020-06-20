#include "mob.h"

#include "stat.h"
#include "spell.h"
#include "item.h"
#include "geography.h"
#include "kill.h"

#include "props.h"
#include "externs.h"

#include "params.h"

#include "debug.h"

#define HUNGER_Y	4
#define THIRST_Y	2
#define HUNGER_INC	(1 << (DAYTICK_Y - HUNGER_Y))
#define THIRST_INC	(1 << (DAYTICK_Y - THIRST_Y))


#define F(x) (1<<(x - 1)) // "folds" value FIXME - 1
#define FIGHTER F(STR) | F(CON) | F(DEX)
#define MOB_DEFINE(s, de) { #s, #s, de }
#define BIRD(s, d) .o = { #s, "bird/" #s, d }, \
	.wt = PECK, .type = ELM_AIR
#define FISH(s, d) .o = { #s, "fish/" #s, d }, \
	.wt = BITE, .type = ELM_ICE

bodypart_t bodypart_map[] = {
	[BP_HEAD] = {
		.name = "head",
	},

	[BP_NECK] = {
		.name = "neck",
	},

	[BP_CHEST] = {
		.name = "chest",
	},

	[BP_BACK] = {
		.name = "back",
	},

	[BP_WEAPON] = {
		.name = "weapon",
	},

	[BP_LFINGER] = {
		.name = "left index finger",
	},

	[BP_RFINGER] = {
		.name = "right index finger",
	},
};

enum bodypart ch_bodypart_map[] = {
	[0 ... 254] = 0,
	['h'] = BP_HEAD,
	['n'] = BP_NECK,
	['c'] = BP_CHEST,
	['b'] = BP_BACK,
	['w'] = BP_WEAPON,
	['l'] = BP_LFINGER,
	['r'] = BP_RFINGER,
	['g'] = BP_LEGS,
};

mobi_t mobi_map[MOBI_SIZE], *mobi_cur = &mobi_map[0];

#include "drop.c"

mob_t mob_map[] = {
	[MOB_HUMAN] = {
		.o = {
			.name = "human",
		},
	},
	[MOB_GOLDFISH] = {
		FISH(goldfish, ""), .y = 4,
	},
	[MOB_SALMON] = {
		FISH(salmon, ""), .y = 4,
	},
	[MOB_TUNA] = {
		FISH(tuna, ""), .y = 6,
	},
	[MOB_KOIFISH] = {
		FISH(koifish, ""), .y = 10,
	},
	[MOB_DOLPHIN] = {
		FISH(dolphin, ""), .y = 13,
	},
	[MOB_SHARK] = {
		FISH(shark, ""), .y = 13, .lvl = 40,
		.lvl_v = 0x1f, .flags = MF_AGGRO,
	},
	[MOB_MOONFISH] = {
		FISH(moonfish, ""), .y = 14, .flags = MF_AGGRO,
	},
	[MOB_RAINBOWFISH] = {
		FISH(rainbowfish, ""), .y = 14, .flags = MF_AGGRO
	},
	[MOB_ICEBIRD] = {
		BIRD(icebird, ""), .y = 14, .type = ELM_ICE, .flags = MF_AGGRO
	},
	[MOB_PARROT] = {
		.y = 1, BIRD(parrot, ""),
	},
	[MOB_BANDIT] = {
		MOB_DEFINE(bandit, ""),
		{ &dagger_drop, ARMORSET_LIST(padded),
			ARMORSET_LIST(hide),
			ARMORSET_LIST(chainmail), NULL },
		4, FIGHTER, 20, 0x7, .flags = MF_AGGRO
	},
	[MOB_SWALLOW] = {
		BIRD(swallow, ""), .y = 4,
	},
	[MOB_SKELETON] = {
		MOB_DEFINE(skeleton, "Bones that almost shine with a white polish that never seems to dull."),
		{ NULL }, 2, FIGHTER, 40, 0xf, .flags = MF_AGGRO, .type = ELM_DARK,
	},
	[MOB_VAMPIRE_SPAWN] = {
		MOB_DEFINE(vampire_spawn, "Undead that came into Being when a Vampire Slew a Mortal."),
		{ NULL }, 6, FIGHTER, 80, 0x1f,
		.type = ELM_VAMP, .flags = MF_AGGRO,
		.o.name = "vampire spawn",
	},
	[MOB_WOODPECKER] = {
		BIRD(woodpecker, ""), .y = 2,
	},
	[MOB_SPARROW] = {
		BIRD(sparrow, ""), .y = 3,
	},
	[MOB_OWL] = {
		BIRD(owl, ""), .y = 7, .type = ELM_DARK,
	},
	[MOB_EAGLE] = {
		BIRD(eagle, ""), .y = 7,
	},
	[MOB_STARLING] = {
		BIRD(starling, ""), .y = 8,
	},
	[MOB_VULTURE] = {
		BIRD(vulture, ""), .y = 7,
	},
	[MOB_FIREBIRD] = {
		BIRD(firebird, ""), .y = 14,
		.type = ELM_FIRE, .flags = MF_AGGRO,
	}
};

static inline void
mob_add_stats(mob_t *mob, dbref nu)
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

static inline dbref
inventory_add(mobi_t *liv, struct item i)
{
	dbref nu;

	nu = obj_add(i.o, liv->who);
	SETEQW(nu, i.eqw);
	SETMSV(nu, i.msv);
	SETRARE(nu, rarity_get());

	if (!cannot_equip(liv->who, nu))
		SETEQ(liv->who, i.eqw, nu);

	return nu;
}

static inline void
mob_inventory(mobi_t *liv, drop_t **drop)
{
	for (; *drop; drop++)
		if (random() < (RAND_MAX >> (*drop)->y))
			inventory_add(liv, (*drop)->i);
}

static inline int
bird_is(mob_t *mob)
{
	return mob->wt == PECK;
}

void
mob_save()
{
	static const unsigned m = MOBI_SIZE;
	register mobi_t *n = MOBI(0);
	char buf[BUFFER_LEN], *b = buf;
	size_t len = 0;
	unsigned i;
	PData mydat;

	// TODO? use mask?
	for (i = 0; i < m; i++, n++) {
		if (n->who > 0 && n->who < db_top && (Typeof(n->who) == TYPE_PLAYER
				   || Typeof(n->who) == TYPE_THING)
                    && GETLID(n->who) >= 0)
		{
			register size_t d;
			d = snprintf(b, sizeof(buf) - len, "#%d ", n->who);
			len += d;
                        CBUG(len > sizeof(buf));
			b += d;
		}
        }

	mydat.flags = PROP_STRTYP;
	mydat.data.str = buf;
	set_property((dbref) 0, "_/living", &mydat);
}

static void
mobi_init(mobi_t *liv, dbref who)
{
	int i;
	mobi_cur = liv;
	memset(liv, 0, sizeof(mobi_t));
	liv->who = who;
	liv->mob = MOB(MOB_HUMAN);
	liv->wts = phys_wts[liv->mob ? liv->mob->wt : GETWTS(who)];
	liv->hunger = liv->thirst = 0;
	liv->flags = 0;
	liv->combo = GETCOMBO(who);
	liv->hp = HP_MAX(who);
	liv->mp = MP_MAX(who);

	MOBI_EV(liv, DMG) = DMG_BASE(who);
	MOBI_EV(liv, DODGE) = DODGE_BASE(who);

	spells_init(liv->spells, who);

	SETLID(who, mobi_cur - mobi_map);

	for (i = 0; i < EQ_MAX; i++) {
		register dbref eq = GETEQ(who, i);
		if (eq > 0)
			equip_calc(liv->who, eq);
	}

	/* debug("mobi_init %d %d\n", who, liv->who); */
}

mobi_t *
mob_put(dbref who)
{
	register const unsigned m = MOBI_SIZE;
	register mobi_t *liv;
	register unsigned i;

	/* debug("mob_put %d\n", who); */

	CBUG(who < 0 || Typeof(who) == TYPE_GARBAGE);

	for (liv = mobi_cur, i = 0;
	     i < m;
	     i++, liv++)
	{
		if (liv > mobi_map + m)
			liv = mobi_map;

		if (liv->who <= 0) {
			/* debug("will mobi_init %d %d %d", who, i, liv->who); */
			mobi_init(liv, who);
			return liv;
		}
	}

	return NULL;
}

static inline dbref
mob_add(enum mob mid, dbref where, enum biome biome, long long pdn) {
	mob_t *mob = MOB(mid);
	drop_t *drop[8];
	mobi_t *liv;
	dbref nu;

	if ((bird_is(mob) && !pdn)
	    || (!NIGHT_IS && (mob->type == ELM_DARK || mob->type == ELM_VAMP))
	    || random() >= (RAND_MAX >> mob->y))
		return NOTHING;

	nu = obj_add(mob->o, where);
	memset(drop, 0, sizeof(drop));

	SETMID(nu, mid);
	SETAGGRO(nu, mob->flags & MF_AGGRO);
	mob_add_stats(mob, nu);
	liv = mob_put(nu);

	/* debug("%d %s loc %d %s MID %d", nu, NAME(nu), where, NAME(where), mid); */

	mob_inventory(liv, (drop_t **) mob->drop);

	if (bird_is(mob)) {
		drop_t feather = { {{ "%s's feather", "", "" }}, 1 };
		char name[32];
		snprintf(name, sizeof(name), feather.i.o.name, NAME(nu));
		feather.i.o.name = name;
		drop[0] = &feather;
	}
	/* else if (fish_is(mob)) { */
		/* drop_t scale = { {{ "%s's scale", "", "" }}, 1 }; */
		/* char name[32]; */
		/* snprintf(name, sizeof(name), scale.i.o.name, NAME(nu)); */
		/* feather.i.o.name = name; */
		/* drop[0] = &feather; */
	/* } */

	mob_inventory(liv, drop);

	return nu;
}

void
mobs_add(dbref w, enum biome biome, long long pdn) {
	unsigned mid,
		 n = 0;
	int o = 0;
	CBUG(w <= 0);

	// TODO more consistent logic that doesn't need
	// me to mess around with this index stuff

	switch (biome) {
	case BIOME_WATER:
		o = MOFS_WATER;
		n = MOFS_ICE;
		break;

	case BIOME_COLD_DRY:
	case BIOME_COLD:
	case BIOME_COLD_WET:
	case BIOME_TUNDRA:
		o = MOFS_ICE;
		n = MOFS_JUNGLE - o;
		break;

	case BIOME_RAIN_FOREST:
		o = MOFS_JUNGLE;
		n = MOFS_TEMPERATE_DESERT - o;
		break;

	case BIOME_TAIGA:
	case BIOME_SHRUBLAND: // / grassland / woodland
	case BIOME_WOODLAND: // yellow tree
	case BIOME_FOREST: // temperate
		o = MOFS_TEMPERATE;
		n = MOFS_DESERT - MOFS_JUNGLE;
		break;

	case BIOME_DESERT:
	case BIOME_SAVANNAH:
		o = MOFS_DESERT;
		n = MOFS_FIRE - o;
		break;

	case BIOME_SEASONAL_FOREST:
	case BIOME_VOLCANIC:
		o = MOFS_FIRE;
		n = MOFS_END - o;
	}

	for (mid = o; mid < o + n; mid++)
		mob_add(mid, w, biome, pdn);
}

struct obj const *
mob_obj_random()
{
	int idx = random() % MOFS_END;
	if (idx == 0 || idx == 5 || idx == 8 || idx == 9)
		return NULL;
	return &MOB(idx)->o;
}

void
mobs_aggro(command_t *cmd)
{
	dbref player = cmd->player;
        mobi_t *me;
	dbref tmp;
	int klock = 0;

        CBUG(GETLID(player) < 0);
        me = MOBI(player);
        if (me->who != player)
                me = mob_put(player);

	DOLIST(tmp, DBFETCH(getloc(player))->contents) {
		int lid = GETLID(tmp);
		if (lid >= 0 && GETAGGRO(tmp)) {
			mobi_t *liv = MOBI(tmp);
			liv->target = me;
			klock++;
		}
	}

	me->klock += klock;
}

void
do_eat(command_t *cmd)
{
	dbref player = cmd->player;
	const char *what = cmd->argv[1];
	mobi_t *p = MOBI(player);
	dbref item = contents_find(cmd, player, what);
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
		recycle(cmd, item);
		return;
	}
	SETSTACK(item, stack - 1);
}

void
mob_init() {
	register PropPtr p = get_property((dbref) 0, "_/living");
        memset(mobi_map, 0, sizeof(mobi_map));

	if (p) {
		const char *liv_s = PropDataStr(p), *l;
		for (l = liv_s; l; l = strchr(l, '#'))
			mob_put((dbref) atoi(++l));
	}
}

static void
respawn(dbref who)
{
	if (who < 0 || Typeof(who) != TYPE_THING)
		return;
	dbref where = THING_HOME(who);
	moveto(who, where);
	notify_except_fmt(DBFETCH(where)->contents, who,
			  "%s appears.", NAME(who));
}

static inline void
huth_notify(dbref who, unsigned v, unsigned char y, char const *m[4])
{
	static unsigned const n[] = {
		1 << (DAY_Y - 1),
		1 << (DAY_Y - 2),
		1 << (DAY_Y - 3)
	};

	register unsigned aux;

	if (v == n[2])
		notify(who, m[0]);
	else if (v == (aux = n[1]))
		notify(who, m[1]);
	else if (v == (aux += n[0]))
		notify(who, m[2]);
	else if (v == (aux += n[2]))
		notify(who, m[3]);
	else if (v > aux) {
		short val = -(HP_MAX(who) >> 3);
		cspell_heal(NOTHING, who, val);
	}
}

static inline void
mobi_update(mobi_t *n)
{
	static char const *thirst_msg[] = {
		"You are thirsty.",
		"You are very thirsty.",
		"you are dehydrated.",
		"You are dying of thirst."
	};

	static char const *hunger_msg[] = {
		"You are hungry.",
		"You are very hungry.",
		"You are starving.",
		"You are starving to death."
	};

	dbref who = n->who;

	if (n->respawn_in > 0) {
		if (!--n->respawn_in)
			respawn(who);

		return;
	} else {
		int y = 9 - 2 * (n->flags | MF_SITTING);

		int max = HP_MAX(who);
		int cur = n->hp + (max >> y);
		n->hp = cur > max ? max : cur;

		max = MP_MAX(who);
		cur = n->mp + (max >> y);
		n->mp = cur > max ? max : cur;

		huth_notify(n->who, n->thirst += THIRST_INC,
			    THIRST_Y, thirst_msg);

		huth_notify(n->who, n->hunger += HUNGER_INC,
			    HUNGER_Y, hunger_msg);
	}

	debufs_process(n->who);
	kill_update(n);
}

void
mob_update()
{
	register mobi_t *n, *e;

	for (n = MOBI(0), e = n + MOBI_SIZE;
	     n < e;
	     n++) {

		if (n->who > 0)
			mobi_update(n);
		else if (n < mobi_cur)
			mobi_cur = n;
	}
}
