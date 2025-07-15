#include <nd/nd.h>

SKEL biomes_skel[] = {
	[BIOME_WATER] = {
                .name = "water",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = BLUE,
                } },
	},

	[BIOME_PERMANENT_ICE] = {
		.name = "permanent ice",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = WHITE,
                } },
	},

	[BIOME_TUNDRA] = {
		.name = "tundra",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = CYAN,
                } },
	},

	[BIOME_TUNDRA2] = {
		.name = "tundra2",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = CYAN,
                } },
	},

	[BIOME_TUNDRA3] = {
		.name = "tundra3",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = CYAN,
                } },
	},

	[BIOME_TUNDRA4] = {
		.name = "tundra4",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = CYAN,
                } },
	},

	[BIOME_COLD_DESERT] = {
		.name = "cold desert",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = CYAN,
                } },
	},

	[BIOME_SHRUBLAND] = {
		.name = "shrubland",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_CONIFEROUS_FOREST] = {
		.name = "coniferous forest",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_BOREAL_FOREST] = {
		.name = "boreal forest",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_TEMPERATE_GRASSLAND] = {
		.name = "temperate grassland",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_WOODLAND] = {
		.name = "woodland",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_TEMPERATE_SEASONAL_FOREST] = {
		.name = "temperate seasonal forest",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_TEMPERATE_RAINFOREST] = {
		.name = "temperate rainforest",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_DESERT] = {
		.name = "desert",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = YELLOW,
                } },
	},

	[BIOME_SAVANNAH] = {
		.name = "savannah",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = YELLOW,
                } },
	},

	[BIOME_TROPICAL_SEASONAL_FOREST] = {
		.name = "tropical seasonal forest",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_TROPICAL_RAINFOREST] = {
		.name = "tropical rainforest",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = BLACK,
                } },
	},

	[BIOME_VOLCANIC] = {
		.name = "volcanic",
                .type = TYPE_ROOM,
                .sp = { .biome = {
                        .bg = BLACK,
                } },
	},
};

void mod_install(void *arg __attribute__((unused))) {
	unsigned biome_map[BIOME_MAX];
	for (unsigned i = 0; i < BIOME_MAX; i++) {
		unsigned biome_ref = nd_put(HD_SKEL, NULL, &biomes_skel[i]);
		biome_map[i] = biome_ref;
	}
	unsigned ref = 16;
	nd_put(HD_BIOME, &ref, biome_map);
}
