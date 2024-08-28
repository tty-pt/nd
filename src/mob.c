#include "mob.h"
#include "uapi/skel.h"
#include "uapi/wts.h"
#include "io.h"
#include "entity.h"
#include "equipment.h"
#include "defaults.h"


#define F(x) (1<<x) // "folds" value FIXME - 1
#define FIGHTER F(ATTR_STR) | F(ATTR_CON) | F(ATTR_DEX)
#define ARMORSET_LIST(s) & s ## _helmet_drop, \
	& s ## _chest_drop, & s ## _pants_drop

#define ARMORSET(s, _type, min_stat, y) \
	SKEL s ## _helmet = { \
		.name = #s " armor helmet", \
		.description = "", \
		.type = STYPE_EQUIPMENT, \
		.sp = { .equipment = { \
			.eqw = EQ(ES_HEAD, _type), \
			.msv = min_stat, \
		} } \
	}; \
	unsigned s ## _helmet_ref = skel_new(&s ## _helmet); \
	DROP s ## _helmet_drop = { y }; \
	unsigned s ## _helmet_drop_ref = drop_new(&s ## _helmet_drop); \
	adrop_add(s ## _helmet_ref, s ## _helmet_drop_ref); \
	SKEL s ## _chest = { \
		.name = #s " armor chest", \
		.description = "", \
		.type = STYPE_EQUIPMENT, \
		.sp = { .equipment = { \
			.eqw = EQ(ES_CHEST, _type), \
			.msv = min_stat, \
		} } \
	}; \
	unsigned s ## _chest_ref = skel_new(&s ## _chest); \
	DROP s ## _chest_drop = { y }; \
	unsigned s ## _chest_drop_ref = drop_new(&s ## _chest_drop); \
	adrop_add(s ## _chest_ref, s ## _chest_drop_ref); \
	SKEL s ## _pants = { \
		.name = #s " armor leggings", \
		.description = "", \
		.type = STYPE_EQUIPMENT, \
		.sp = { .equipment = { \
			.eqw = EQ(ES_PANTS, _type), \
			.msv = min_stat, \
		} } \
	}; \
	unsigned s ## _pants_ref = skel_new(&s ## _pants); \
	DROP s ## _pants_drop = { y }; \
	unsigned s ## _pants_drop_ref = drop_new(&s ## _pants_drop); \
	adrop_add(s ## _pants_ref, s ## _pants_drop_ref);

#define ARMORSET_LIGHT(s, min_dex, y) \
	ARMORSET(s, ARMOR_LIGHT, min_dex, y)

#define ARMORSET_MEDIUM(s, min_stat, y) \
	ARMORSET(s, ARMOR_MEDIUM, min_stat, y)

#define ARMORSET_HEAVY(s, min_str, y) \
	ARMORSET(s, ARMOR_HEAVY, min_str, y)

SKEL dagger = {
	.name = "dagger",
	.description = "",
	.type = STYPE_EQUIPMENT,
	.sp = { .equipment = {
		.eqw = EQ(ES_RHAND, WT_SLASH),
		.msv = 5
	} }
};

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

SKEL nodrop_skel[] = {
	[MOB_HUMAN] = {
		.name = "human",
	},
	[MOB_GOLDFISH] = {
		.name = "goldfish",
		.description = "",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 4,
			}
		},
	},
	[MOB_SALMON] = {
		.name = "salmon",
		.description = "",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 4,
			}
		},
	},
	[MOB_TUNA] = {
		.name = "tuna",
		.description = "",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 6,
			}
		},
	},
	[MOB_KOIFISH] = {
		.name = "koifish",
		.description = "",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 10,
			}
		},
	},
	[MOB_DOLPHIN] = {
		.name = "dolphin",
		.description = "",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
				.type = ELM_ICE,
				.biomes = (1<<BIOME_WATER),
				.y = 13,
			}
		},
	},
	[MOB_SHARK] = {
		.name = "shark",
		.description = "",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
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
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
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
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
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
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_PECK,
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
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_PECK,
				.type = ELM_AIR,
				.biomes = (1 << BIOME_TEMPERATE_RAINFOREST),
				.y = 4,
				.flags = EF_AGGRO,
			}
		},
	},
	[MOB_BANDIT] = {
		.name = "bandit",
		.description = "A shady person under some robes",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
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
		.description = "",
		.type = STYPE_ENTITY,
		.sp = { .entity = {
			.wt = WT_PECK,
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
		.type = STYPE_ENTITY,
		.sp = { .entity = {
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
		.description = "",
		.type = STYPE_ENTITY,
		.sp = { .entity = {
			.wt = WT_PECK,
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
		.description = "",
		.type = STYPE_ENTITY,
		.sp = { .entity = {
			.wt = WT_PECK, .type = ELM_AIR,
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
	[MOB_OWL] = {
		.name = "owl",
		.description = "",
		.type = STYPE_ENTITY,
		.sp = { .entity = {
			.wt = WT_PECK, .type = ELM_DARK,
			.y = 7,
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

unsigned mob_refs[MOB_MAX];

void mobs_init() {
	for (unsigned i = 0; i < MOB_MAX; i++)
		mob_refs[i] = skel_new(&nodrop_skel[i]);

	ARMORSET_LIGHT(padded, 15, 5);
	ARMORSET_MEDIUM(hide, 15, 5);
	ARMORSET_HEAVY(chainmail, 15, 5);

	/* unsigned dagger_drop_ref = drop_new(&dagger_drop); */
	/* adrop_add(bandit_ref, dagger_drop_ref); */

	unsigned bandit_ref  = mob_refs[MOB_BANDIT];
	adrop_add(bandit_ref, padded_helmet_drop_ref);
	adrop_add(bandit_ref, padded_chest_drop_ref);
	adrop_add(bandit_ref, padded_pants_drop_ref);
	adrop_add(bandit_ref, hide_helmet_drop_ref);
	adrop_add(bandit_ref, hide_chest_drop_ref);
	adrop_add(bandit_ref, hide_pants_drop_ref);
	adrop_add(bandit_ref, chainmail_helmet_drop_ref);
	adrop_add(bandit_ref, chainmail_chest_drop_ref);
	adrop_add(bandit_ref, chainmail_pants_drop_ref);
}
