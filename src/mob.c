#include "mob.h"

#include "stat.h"
#include "spell.h"
#include "item.h"
#include "geography.h"
#include "kill.h"

#include "props.h"
#include "externs.h"

#include "params.h"

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
	.wt = BITE, .type = ELM_ICE, .biomes = (1<<BIOME_WATER)

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

#include "drop.c"

struct object_skeleton mob_skeleton_map[] = {
	[MOB_HUMAN] = {
		.name = "human",
	},
	[MOB_GOLDFISH] = {
		.name = "goldfish",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_MOB,
		.sp = {
			.mob = {
				.wt = BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 4,
			}
		},
	},
	[MOB_SALMON] = {
		.name = "salmon",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_MOB,
		.sp = {
			.mob = {
				.wt = BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 4,
			}
		},
	},
	[MOB_TUNA] = {
		.name = "tuna",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_MOB,
		.sp = {
			.mob = {
				.wt = BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 6,
			}
		},
	},
	[MOB_KOIFISH] = {
		.name = "koifish",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_MOB,
		.sp = {
			.mob = {
				.wt = BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 10,
			}
		},
	},
	[MOB_DOLPHIN] = {
		.name = "dolphin",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_MOB,
		.sp = {
			.mob = {
				.wt = BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 13,
			}
		},
	},
	[MOB_SHARK] = {
		.name = "shark",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_MOB,
		.sp = {
			.mob = {
				.wt = BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 13,
				.lvl = 40,
				.lvl_v = 0x1f,
				.flags = MF_AGGRO,
			}
		},
	},
	[MOB_MOONFISH] = {
		.name = "moonfish",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_MOB,
		.sp = {
			.mob = {
				.wt = BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 14,
				.flags = MF_AGGRO,
			}
		},
	},
	[MOB_RAINBOWFISH] = {
		.name = "rainbowfish",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_MOB,
		.sp = {
			.mob = {
				.wt = BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 14,
				.flags = MF_AGGRO,
			}
		},
	},
	[MOB_ICEBIRD] = {
		.name = "icebird",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_MOB,
		.sp = {
			.mob = {
				.wt = PECK,
				.type = ELM_AIR,
				.biomes = (1 << BIOME_PERMANENT_ICE)
					| (1 << BIOME_TUNDRA)
					| (1 << BIOME_TUNDRA2)
					| (1 << BIOME_TUNDRA3)
					| (1 << BIOME_TUNDRA4)
					| (1 << BIOME_COLD_DESERT),
				.y = 14,
				.flags = MF_AGGRO,
			}
		},

	},
	[MOB_PARROT] = {
		.name = "parrot",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_MOB,
		.sp = {
			.mob = {
				.wt = PECK,
				.type = ELM_AIR,
				.biomes = (1 << BIOME_TEMPERATE_RAINFOREST),
				.y = 4,
				.flags = MF_AGGRO,
			}
		},
	},
	[MOB_BANDIT] = {
		.name = "bandit",
		.art = "bandit.jpg",
		.description = "A shady person under some robes",
		.avatar = "bandit_avatar.jpg",
		.type = S_TYPE_MOB,
		.sp = {
			.mob = {
				.drop = {
					&dagger_drop,
					ARMORSET_LIST(padded),
					ARMORSET_LIST(hide),
					ARMORSET_LIST(chainmail),
					NULL
				},
				.y = 4,
				.stat = FIGHTER,
				.lvl = 5,
				.lvl_v = 0x7,
				.flags = MF_AGGRO,
				.biomes = (1 << BIOME_SHRUBLAND)
					| (1 << BIOME_CONIFEROUS_FOREST)
					| (1 << BIOME_BOREAL_FOREST)
					| (1 << BIOME_TEMPERATE_GRASSLAND)
					| (1 << BIOME_WOODLAND)
					| (1 << BIOME_TEMPERATE_SEASONAL_FOREST)
					| (1 << BIOME_TEMPERATE_RAINFOREST),
			}
		},
	},
	[MOB_SWALLOW] = {
		.name = "swallow",
		.art = "swallow.png",
		.description = "",
		.avatar = "swallow_avatar.png",
		.type = S_TYPE_MOB,
		.sp = { .mob = {
			.wt = PECK,
			.type = ELM_AIR,
			.y = 4,
			.biomes = (1 << BIOME_SHRUBLAND)
				| (1 << BIOME_CONIFEROUS_FOREST)
				| (1 << BIOME_BOREAL_FOREST)
				| (1 << BIOME_TEMPERATE_GRASSLAND)
				| (1 << BIOME_WOODLAND)
				| (1 << BIOME_TEMPERATE_SEASONAL_FOREST)
				| (1 << BIOME_TEMPERATE_RAINFOREST)
		} },
	},
	/* [MOB_SKELETON] = { */
	/* 	.name = "skeleton", */
	/* 	.description = "Bones that almost shine with a white polish that never seems to dull.", */
	/* 	.type = S_TYPE_MOB, */
	/* 	.sp = { .mob = { */
	/* 		.drop = { NULL }, */
	/* 		.y = 2, */
	/* 		.stat = FIGHTER, */
	/* 		.lvl = 40, */
	/* 		.lvl_v = 0xf, */
	/* 		.flags = MF_AGGRO, */
	/* 		.type = ELM_DARK, */
	/* 	} }, */
	/* }, */
	[MOB_VAMPIRE_SPAWN] = {
		.name = "vampire spawn",
		.description = "Undead that came into Being when a Vampire Slew a Mortal.",
		.art = "",
		.avatar = "",
		.type = S_TYPE_MOB,
		.sp = { .mob = {
			.drop = { NULL },
			.y = 6,
			.stat = FIGHTER,
			.lvl = 80,
			.lvl_v = 0x1f,
			.type = ELM_VAMP,
			.flags = MF_AGGRO,
		} },
	},
	[MOB_WOODPECKER] = {
		.name = "woodpecker",
		.art = "woodpecker.jpg",
		.description = "",
		.avatar = "woodpecker_avatar.jpg",
		.type = S_TYPE_MOB,
		.sp = { .mob = {
			.wt = PECK,
			.type = ELM_AIR,
			.y = 2,
			.biomes = (1 << BIOME_SHRUBLAND)
				| (1 << BIOME_CONIFEROUS_FOREST)
				| (1 << BIOME_BOREAL_FOREST)
				| (1 << BIOME_TEMPERATE_GRASSLAND)
				| (1 << BIOME_WOODLAND)
				| (1 << BIOME_TEMPERATE_SEASONAL_FOREST)
				| (1 << BIOME_TEMPERATE_RAINFOREST),
		} },
	},
	[MOB_SPARROW] = {
		.name = "sparrow",
		.art = "sparrow.jpg",
		.description = "",
		.avatar = "sparrow_avatar.jpg",
		.type = S_TYPE_MOB,
		.sp = { .mob = {
			.wt = PECK, .type = ELM_AIR,
			.y = 3,
			.biomes = (1 << BIOME_SHRUBLAND)
				| (1 << BIOME_CONIFEROUS_FOREST)
				| (1 << BIOME_BOREAL_FOREST)
				| (1 << BIOME_TEMPERATE_GRASSLAND)
				| (1 << BIOME_WOODLAND)
				| (1 << BIOME_TEMPERATE_SEASONAL_FOREST)
				| (1 << BIOME_TEMPERATE_RAINFOREST)
		} },
	},
	/* [MOB_OWL] = { */
	/* 	BIRD(owl, ""), .y = 7, .type = ELM_DARK, */
	/* 	.biomes = (1 << BIOME_SHRUBLAND) */
	/* 		| (1 << BIOME_CONIFEROUS_FOREST) */
	/* 		| (1 << BIOME_BOREAL_FOREST) */
	/* 		| (1 << BIOME_TEMPERATE_GRASSLAND) */
	/* 		| (1 << BIOME_WOODLAND) */
	/* 		| (1 << BIOME_TEMPERATE_SEASONAL_FOREST) */
	/* 		| (1 << BIOME_TEMPERATE_RAINFOREST) */
	/* }, */
	/* [MOB_EAGLE] = { */
	/* 	BIRD(eagle, ""), .y = 7, */
	/* }, */
	/* [MOB_STARLING] = { */
	/* 	BIRD(starling, ""), .y = 8, */
	/* }, */
	/* [MOB_VULTURE] = { */
	/* 	BIRD(vulture, ""), .y = 7, */
	/* }, */
	/* [MOB_FIREBIRD] = { */
	/* 	BIRD(firebird, ""), .y = 14, */
	/* 	.type = ELM_FIRE, .flags = MF_AGGRO, */
	/* } */
};

void
mob_add_stats(struct object_skeleton *mob, dbref nu)
{
	unsigned char stat = mob->sp.mob.stat;
	int lvl = mob->sp.mob.lvl, spend, i, sp,
	    v = mob->sp.mob.lvl_v ? mob->sp.mob.lvl_v : 0xf;

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
bird_is(struct mob_skeleton *mob)
{
	return mob->wt == PECK;
}

struct mob *
mob_put(dbref who)
{
	register struct mob *mob;
        struct object *o;

        /* CBUG(GETLID(who) >= 0); */

        mob = (struct mob *) malloc(sizeof(struct mob));

        CBUG(!mob);

	o = DBFETCH(who);
        o->mob = mob;

	memset(mob, 0, sizeof(struct mob));
	mob->wts = phys_wts[GETWTS(who)];
	mob->hunger = mob->thirst = 0;
	mob->flags = 0;
	mob->combo = GETCOMBO(who);
	mob->hp = HP_MAX(who);
	mob->mp = MP_MAX(who);

	MOB_EV(mob, DMG) = DMG_BASE(who);
	MOB_EV(mob, DODGE) = DODGE_BASE(who);

	spells_init(mob->spells, who);

	SETLID(who, 1);

	int i;

	for (i = 0; i < EQ_MAX; i++) {
		register dbref eq = GETEQ(who, i);
		if (eq > 0)
			CBUG(equip_affect(who, eq));
	}

        return mob;
}

static inline dbref
mob_add(enum mob_type mid, dbref where, enum biome biome, long long pdn) {
	struct object_skeleton *obj_skel = MOB_SKELETON(mid);
	CBUG(obj_skel->type != S_TYPE_MOB);
	struct mob_skeleton *mob_skel = &obj_skel->sp.mob;
	dbref nu;

	if ((bird_is(mob_skel) && !pdn)
	    || (!NIGHT_IS && (mob_skel->type == ELM_DARK || mob_skel->type == ELM_VAMP))
	    || random() >= (RAND_MAX >> mob_skel->y))
		return NOTHING;

	if (!((1 << biome) & mob_skel->biomes))
		return NOTHING;

	nu = object_add(*obj_skel, where);
	return nu;
}

void
mobs_add(dbref w, enum biome biome, long long pdn) {
	/* int o = MOFS_ICE; */
	int o = 1;
	unsigned mid,
		 n = (sizeof(mob_skeleton_map) / sizeof(struct object_skeleton)) - 1;
	CBUG(w <= 0);

	for (mid = o; mid < o + n; mid++) {
		mob_add(mid, w, biome, pdn);
	}
}

struct object_skeleton const *
mob_obj_random()
{
	int idx = random() % MOFS_END;
	if (idx == 0 || idx == 5 || idx == 8 || idx == 9)
		return NULL;
	return MOB_SKELETON(idx);
}

void
mobs_aggro(dbref player)
{
        struct mob *me = MOB(player);
	dbref tmp;
	int klock = 0;

        CBUG(GETLID(player) < 0);

	DOLIST(tmp, DBFETCH(getloc(player))->contents) {
		int lid = GETLID(tmp);
		if (lid >= 0 && GETAGGRO(tmp)) {
			struct mob *mob = MOB(tmp);
			mob->target = player;
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
	struct mob *p = MOB(player);
	dbref item = contents_find(player, player, what);
	int food;

	if (item < 0
	    || (food = GETFOOD(item)) < 0) {
		notify(player, "You can't eat that.");
		return;
	}

	// TODO food should have some water content
	food = p->hunger - (1 << (16 - food));
	if (food < 0)
		food = 0;
	p->hunger = food;
	notify_wts(player, "eat", "eats", " %s", NAME(item));
        recycle(player, item);
}

static void
respawn(dbref who)
{
	if (who < 0 || Typeof(who) != TYPE_THING)
		return;
	dbref where = THING_HOME(who);
	moveto(who, where);
        struct mob *mob = MOB(who);
        mob->hp = HP_MAX(who);
        mob->mp = MP_MAX(who);
	notify_except_fmt(DBFETCH(where)->contents, who,
			  "%s appears.", NAME(who));
}

static inline int
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
		return cspell_heal(NOTHING, who, val);
	}

        return 0;
}

void
mob_update(dbref who, long long unsigned tick)
{
        struct mob *n = MOB(who);
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

	if (n->respawn_in > 0) {
		if (!--n->respawn_in)
			respawn(who);

		return;
	}

        if (Typeof(who) != TYPE_PLAYER) {
                if (GETSAT(who) == NOTHING) {
                        if (n->hp != HP_MAX(who)
                            && n->target == NOTHING)
                                sit(who, "");
                } else if (n->hp == HP_MAX(who))
                        stand(who);
        }

	if (tick % 16 == 0 && GETSAT(who) != NOTHING) {
		int div = 10;
		int max, cur;
		cspell_heal(NOTHING, who, HP_MAX(who) / div);

		max = MP_MAX(who);
		cur = n->mp + (max / div);
		n->mp = cur > max ? max : cur;

	}

        CBUG(Typeof(who) == TYPE_GARBAGE);

        /* if mob dies, return */
	if (huth_notify(who, n->thirst += THIRST_INC, THIRST_Y, thirst_msg)
                || huth_notify(who, n->hunger += HUNGER_INC, HUNGER_Y, hunger_msg)
                || debufs_process(who))
                        return;

	kill_update(who);

}
