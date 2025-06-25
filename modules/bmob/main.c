#include <nd/nd.h>

enum wt {
	WT_BITE,
	WT_PECK,
};

enum legacy_mob_type {
	MOB_GOLDFISH,
	MOB_SALMON,
	MOB_TUNA,
	MOB_KOIFISH,
	MOB_DOLPHIN,
	MOB_SHARK,
	MOB_MOONFISH,
	MOB_RAINBOWFISH,
	MOB_ICEBIRD,
	MOB_PARROT,
	MOB_SWALLOW,
	MOB_WOODPECKER,
	MOB_SPARROW,
	MOB_OWL,
#if 0
	MOB_EAGLE,
	MOB_STARLING,
	MOB_VULTURE,
	MOB_FIREBIRD,
#endif
	MOB_MAX,
};

SKEL mob_skel[] = {
	[MOB_GOLDFISH] = {
		.name = "goldfish",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
				.element = ELM_WATER,
				.biomes = (1<<BIOME_WATER),
				.y = 4,
			}
		},
	},
	[MOB_SALMON] = {
		.name = "salmon",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
				.element = ELM_WATER,
				.biomes = (1<<BIOME_WATER),
				.y = 4,
			}
		},
	},
	[MOB_TUNA] = {
		.name = "tuna",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
				.element = ELM_WATER,
				.biomes = (1<<BIOME_WATER),
				.y = 6,
			}
		},
	},
	[MOB_KOIFISH] = {
		.name = "koifish",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
				.element = ELM_WATER,
				.biomes = (1<<BIOME_WATER),
				.y = 10,
			}
		},
	},
	[MOB_DOLPHIN] = {
		.name = "dolphin",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
				.element = ELM_WATER,
				.biomes = (1<<BIOME_WATER),
				.y = 13,
			}
		},
	},
	[MOB_SHARK] = {
		.name = "shark",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
				.element = ELM_WATER,
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
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
				.element = ELM_WATER,
				.biomes = (1<<BIOME_WATER),
				.y = 14,
				.flags = EF_AGGRO,
			}
		},
	},
	[MOB_RAINBOWFISH] = {
		.name = "rainbowfish",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_BITE,
				.element = ELM_WATER,
				.biomes = (1<<BIOME_WATER),
				.y = 14,
				.flags = EF_AGGRO,
			}
		},
	},
	[MOB_ICEBIRD] = {
		.name = "icebird",
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_PECK,
				.element = ELM_AIR,
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
		.type = STYPE_ENTITY,
		.sp = {
			.entity = {
				.wt = WT_PECK,
				.element = ELM_AIR,
				.biomes = (1 << BIOME_TEMPERATE_RAINFOREST),
				.y = 4,
				.flags = EF_AGGRO,
			}
		},
	},
	[MOB_SWALLOW] = {
		.name = "swallow",
		.type = STYPE_ENTITY,
		.sp = { .entity = {
			.wt = WT_PECK,
			.element = ELM_AIR,
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
	[MOB_WOODPECKER] = {
		.name = "woodpecker",
		.type = STYPE_ENTITY,
		.sp = { .entity = {
			.wt = WT_PECK,
			.element = ELM_AIR,
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
		.type = STYPE_ENTITY,
		.sp = { .entity = {
			.wt = WT_PECK, .element = ELM_AIR,
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
		.type = STYPE_ENTITY,
		.sp = { .entity = {
			.wt = WT_PECK, .element = ELM_AIR,
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
#if 0
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
		.element = ELM_FIRE, .flags = EF_AGGRO,
	}
#endif
};

char *wts[] = {
	[WT_PECK] = "peck",
	[WT_BITE] = "bite",
};

unsigned wt_refs[2];
unsigned mob_refs[MOB_MAX];

static inline int
bird_is(SENT *sk)
{
	return sk->wt == wt_refs[WT_PECK];
}

static inline unsigned
mob_add(enum legacy_mob_type mid, unsigned where_ref, enum biome biome, long long pdn) {
	unsigned skel_ref = mob_refs[mid];
	SKEL skel = mob_skel[mid];
	SENT *mob_skel = &skel.sp.entity;

	if ((bird_is(mob_skel) && !pdn)
	    // || (!NIGHT_IS && (mob_skel->type == ELM_DARK || mob_skel->type == ELM_VAMP))
	    || random() >= (RAND_MAX >> mob_skel->y))
		return NOTHING;

	if (!((1 << biome) & mob_skel->biomes))
		return NOTHING;

	OBJ obj;
	unsigned obj_ref = object_add(&obj, skel_ref, where_ref, NULL);
	nd_put(HD_OBJ, &obj_ref, &obj);
	return obj_ref;
}

void
mod_install(void) {
	unsigned mid;

	if (nd_get(HD_RWTS, &wt_refs[WT_BITE], "bite"))
		wt_refs[WT_BITE] = nd_put(HD_WTS, NULL, "bite");

	if (nd_get(HD_RWTS, &wt_refs[WT_PECK], "peck"))
		wt_refs[WT_PECK] = nd_put(HD_WTS, NULL, "peck");

	for (mid = 0; mid < MOB_MAX; mid++) {
		SKEL *skel = &mob_skel[mid];
		unsigned wt = skel->sp.entity.wt; // attention
		skel->sp.entity.wt = wt_refs[wt];
		mob_refs[mid] = nd_put(HD_SKEL, NULL, skel);
	}
}

void
mob_open(void) {
	// when opening dbs, we still need this:
	nd_get(HD_RWTS, &wt_refs[WT_PECK], "peck");
}

void
mod_spawn(void *arg) {
	struct spawn_arg *sa = arg;
	struct bio *bio = sa->arg;
	for (unsigned mid = 0; mid < MOB_MAX; mid++)
		mob_add(mid, sa->where_ref, bio->bio_idx, bio->pd.n);
}
