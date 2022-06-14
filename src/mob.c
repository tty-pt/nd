#include "mob.h"
#include "io.h"
#include "spacetime.h"
#include "entity.h"
#include "equipment.h"
#include "spell.h"
#include "defaults.h"
#include "props.h"
#include "externs.h"
#include "params.h"


#define F(x) (1<<x) // "folds" value FIXME - 1
#define FIGHTER F(ATTR_STR) | F(ATTR_CON) | F(ATTR_DEX)
#define MOB_DEFINE(s, de) { #s, #s, de }
#define BIRD(s, d) .o = { #s, "bird/" #s, d }, \
	.wt = PECK, .type = ELM_AIR
#define FISH(s, d) .o = { #s, "fish/" #s, d }, \
	.wt = BITE, .type = ELM_ICE, .biomes = (1<<BIOME_WATER)
#define ARMORSET_LIST(s) & s ## _helmet_drop, \
	& s ## _chest_drop, & s ## _pants_drop

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

