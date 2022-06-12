#include "io.h"

#include "mob.h"
#include "entity.h"

#include "stat.h"
#include "spell.h"
#include "item.h"
#include "geography.h"
#include "kill.h"
#include "defaults.h"

#include "props.h"
#include "externs.h"

#include "params.h"

#define HUNGER_Y	4
#define THIRST_Y	2
#define HUNGER_INC	(1 << (DAYTICK_Y - HUNGER_Y))
#define THIRST_INC	(1 << (DAYTICK_Y - THIRST_Y))


#define F(x) (1<<x) // "folds" value FIXME - 1
#define FIGHTER F(ATTR_STR) | F(ATTR_CON) | F(ATTR_DEX)
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

struct object_skeleton entity_skeleton_map[] = {
	[MOB_HUMAN] = {
		.name = "human",
	},
	[MOB_GOLDFISH] = {
		.name = "goldfish",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_ENTITY,
		.sp = {
			.entity = {
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
		.type = S_TYPE_ENTITY,
		.sp = {
			.entity = {
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
		.type = S_TYPE_ENTITY,
		.sp = {
			.entity = {
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
		.type = S_TYPE_ENTITY,
		.sp = {
			.entity = {
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
		.type = S_TYPE_ENTITY,
		.sp = {
			.entity = {
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
		.type = S_TYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 13,
				.lvl = 40,
				.lvl_v = 0x1f,
				.flags = EF_AGGRO,
			}
		},
	},
	[MOB_MOONFISH] = {
		.name = "moonfish",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 14,
				.flags = EF_AGGRO,
			}
		},
	},
	[MOB_RAINBOWFISH] = {
		.name = "rainbowfish",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 14,
				.flags = EF_AGGRO,
			}
		},
	},
	[MOB_ICEBIRD] = {
		.name = "icebird",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = PECK,
				.type = ELM_AIR,
				.biomes = (1 << BIOME_PERMANENT_ICE)
					| (1 << BIOME_TUNDRA)
					| (1 << BIOME_TUNDRA2)
					| (1 << BIOME_TUNDRA3)
					| (1 << BIOME_TUNDRA4)
					| (1 << BIOME_COLD_DESERT),
				.y = 14,
				.flags = EF_AGGRO,
			}
		},

	},
	[MOB_PARROT] = {
		.name = "parrot",
		.description = "",
		.art = "",
		.avatar = "",
		.type = S_TYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = PECK,
				.type = ELM_AIR,
				.biomes = (1 << BIOME_TEMPERATE_RAINFOREST),
				.y = 4,
				.flags = EF_AGGRO,
			}
		},
	},
	[MOB_BANDIT] = {
		.name = "bandit",
		.art = "bandit.jpg",
		.description = "A shady person under some robes",
		.avatar = "bandit_avatar.jpg",
		.type = S_TYPE_ENTITY,
		.sp = {
			.entity = {
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
				.flags = EF_AGGRO,
				.biomes = 0,
				/* .biomes = (1 << BIOME_SHRUBLAND) */
				/* 	| (1 << BIOME_CONIFEROUS_FOREST) */
				/* 	| (1 << BIOME_BOREAL_FOREST) */
				/* 	| (1 << BIOME_TEMPERATE_GRASSLAND) */
				/* 	| (1 << BIOME_WOODLAND) */
				/* 	| (1 << BIOME_TEMPERATE_SEASONAL_FOREST) */
				/* 	| (1 << BIOME_TEMPERATE_RAINFOREST), */
			}
		},
	},
	[MOB_SWALLOW] = {
		.name = "swallow",
		.art = "swallow.png",
		.description = "",
		.avatar = "swallow_avatar.png",
		.type = S_TYPE_ENTITY,
		.sp = { .entity = {
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
	/* 	.type = S_TYPE_ENTITY, */
	/* 	.sp = { .entity = { */
	/* 		.drop = { NULL }, */
	/* 		.y = 2, */
	/* 		.stat = FIGHTER, */
	/* 		.lvl = 40, */
	/* 		.lvl_v = 0xf, */
	/* 		.flags = EF_AGGRO, */
	/* 		.type = ELM_DARK, */
	/* 	} }, */
	/* }, */
	[MOB_VAMPIRE_SPAWN] = {
		.name = "vampire spawn",
		.description = "Undead that came into Being when a Vampire Slew a Mortal.",
		.art = "",
		.avatar = "",
		.type = S_TYPE_ENTITY,
		.sp = { .entity = {
			.drop = { NULL },
			.y = 6,
			.stat = FIGHTER,
			.lvl = 80,
			.lvl_v = 0x1f,
			.type = ELM_VAMP,
			.flags = EF_AGGRO,
		} },
	},
	[MOB_WOODPECKER] = {
		.name = "woodpecker",
		.art = "woodpecker.jpg",
		.description = "",
		.avatar = "woodpecker_avatar.jpg",
		.type = S_TYPE_ENTITY,
		.sp = { .entity = {
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
		.type = S_TYPE_ENTITY,
		.sp = { .entity = {
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
	/* 	.type = ELM_FIRE, .flags = EF_AGGRO, */
	/* } */
};

void
stats_init(ENT *enu, SENT *sk)
{
	unsigned char stat = sk->stat;
	int lvl = sk->lvl, spend, i, sp,
	    v = sk->lvl_v ? sk->lvl_v : 0xf;

	lvl += random() & v;

	if (!stat)
		stat = 0x1f;

	spend = 1 + lvl;
	for (i = 0; i < ATTR_MAX; i++)
		if (stat & (1<<i)) {
			sp = random() % spend;
			enu->attr[i] = sp;
		}

	enu->lvl = lvl;
}

static inline int
bird_is(SENT *sk)
{
	return sk->wt == PECK;
}

ENT *
birth(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
	eplayer->wts = phys_wts[eplayer->wtso];
	eplayer->hunger = eplayer->thirst = 0;
	eplayer->combo = 0;
	eplayer->hp = HP_MAX(eplayer);
	eplayer->mp = MP_MAX(eplayer);
	eplayer->sat = eplayer->target = NULL;

	EFFECT(eplayer, DMG).value = DMG_BASE(eplayer);
	EFFECT(eplayer, DODGE).value = DODGE_BASE(eplayer);

	spells_init(eplayer->spells, player);

	int i;

	for (i = 0; i < ES_MAX; i++) {
		register dbref eq = EQUIP(eplayer, i);

		if (eq <= 0)
			continue;

		OBJ *oeq = object_get(eq);
		EQU *eeq = &oeq->sp.equipment;
		CBUG(equip_affect(eplayer, eeq));
	}

        return eplayer;
}

static inline OBJ *
mob_add(enum mob_type mid, OBJ *where, enum biome biome, long long pdn) {
	struct object_skeleton *obj_skel = ENTITY_SKELETON(mid);
	CBUG(obj_skel->type != S_TYPE_ENTITY);
	struct entity_skeleton *mob_skel = &obj_skel->sp.entity;

	if ((bird_is(mob_skel) && !pdn)
	    || (!NIGHT_IS && (mob_skel->type == ELM_DARK || mob_skel->type == ELM_VAMP))
	    || random() >= (RAND_MAX >> mob_skel->y))
		return NULL;

	if (!((1 << biome) & mob_skel->biomes))
		return NULL;

	return object_add(obj_skel, where);
}

void
entities_add(OBJ *where, enum biome biome, long long pdn) {
	/* int o = MOFS_ICE; */
	int o = 1;
	unsigned mid,
		 n = (sizeof(entity_skeleton_map) / sizeof(struct object_skeleton)) - 1;

	for (mid = o; mid < o + n; mid++)
		mob_add(mid, where, biome, pdn);
}

struct object_skeleton const *
mob_obj_random()
{
	int idx = random() % MOFS_END;
	if (idx == 0 || idx == 5 || idx == 8 || idx == 9)
		return NULL;
	return ENTITY_SKELETON(idx);
}

void
mobs_aggro(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
	OBJ *here = player->location;
	OBJ *tmp;
	int klock = 0;

	FOR_LIST(tmp, here->contents) {
		if (tmp->type != TYPE_ENTITY)
			continue;

		ENT *etmp = &tmp->sp.entity;

		if (etmp->flags & EF_AGGRO) {
			etmp->target = player;
			klock++;
		}
	}

	eplayer->klock += klock;
}

static void
respawn(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
	OBJ *where;

	onotifyf(player, "%s disappears.", player->name);

	if (eplayer->flags & EF_PLAYER) {
		struct cmd_dir cd;
		cd.rep = STARTING_POSITION;
		cd.dir = '\0';
		geo_teleport(player, cd);
		where = player->location;
	} else {
		where = eplayer->home;
		/* warn("respawning %d to %d\n", who, where); */
		object_move(player, where);
	}

	onotifyf(player, "%s appears.", player->name);
}

static inline int
huth_notify(OBJ *player, unsigned v, unsigned char y, char const *m[4])
{
	ENT *eplayer = &player->sp.entity;

	static unsigned const n[] = {
		1 << (DAY_Y - 1),
		1 << (DAY_Y - 2),
		1 << (DAY_Y - 3)
	};

	register unsigned aux;

	if (v == n[2])
		notify(eplayer, m[0]);
	else if (v == (aux = n[1]))
		notify(eplayer, m[1]);
	else if (v == (aux += n[0]))
		notify(eplayer, m[2]);
	else if (v == (aux += n[2]))
		notify(eplayer, m[3]);
	else if (v > aux) {
		short val = -(HP_MAX(eplayer) >> 3);
		return entity_damage(NULL, player, val);
	}

        return 0;
}

void
entity_update(OBJ *player)
{
	CBUG(player->type != TYPE_ENTITY);
	ENT *eplayer = &player->sp.entity;

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

	if (!(eplayer->flags & EF_PLAYER)) {
		/* warn("%d hp %d/%d\n", player, eplayer->hp, HP_MAX(player)); */
		if (eplayer->hp == HP_MAX(eplayer)) {
			/* warn("%d's hp is maximum\n", player); */

			if ((eplayer->flags & EF_SITTING)) {
				/* warn("%d is sitting so they shall stand\n", player); */
				stand(player);
			}

			if (player->location == 0) {
				/* warn("%d is located at 0, so they shall respawn\n", player); */
				respawn(player);
			}
		} else {
			/* warn("%d's hp is not maximum\n", player); */
			if (eplayer->target && !(eplayer->flags & EF_SITTING)) {
				/* warn("%d is not sitting so they shall sit\n", player); */
				sit(player, "");
			}
		}
	}

	if (get_tick() % 16 == 0 && (eplayer->flags & EF_SITTING)) {
		int div = 10;
		int max, cur;
		entity_damage(NULL, player, HP_MAX(eplayer) / div);

		max = MP_MAX(eplayer);
		cur = eplayer->mp + (max / div);
		eplayer->mp = cur > max ? max : cur;

	}

        CBUG(player->type == TYPE_GARBAGE);

	if (player->location == 0)
		return;

        /* if mob dies, return */
	if (huth_notify(player, eplayer->thirst += THIRST_INC, THIRST_Y, thirst_msg)
                || huth_notify(player, eplayer->hunger += HUNGER_INC, HUNGER_Y, hunger_msg)
                || debufs_process(player))
                        return;

	kill_update(player);
}
