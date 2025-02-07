#include <uapi/nd.h>
#include "./bbiome.h"

SKEL biomes_skel[] = {
	[BIOME_WATER] = {
                .name = "water",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = BLUE,
                } },
	},

	[BIOME_PERMANENT_ICE] = {
		.name = "permanent ice",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = WHITE,
                } },
	},

	[BIOME_TUNDRA] = {
		.name = "tundra",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = CYAN,
                } },
	},

	[BIOME_TUNDRA2] = {
		.name = "tundra2",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = CYAN,
                } },
	},

	[BIOME_TUNDRA3] = {
		.name = "tundra3",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = CYAN,
                } },
	},

	[BIOME_TUNDRA4] = {
		.name = "tundra4",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = CYAN,
                } },
	},

	[BIOME_COLD_DESERT] = {
		.name = "cold desert",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = CYAN,
                } },
	},

	[BIOME_SHRUBLAND] = {
		.name = "shrubland",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_CONIFEROUS_FOREST] = {
		.name = "coniferous forest",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_BOREAL_FOREST] = {
		.name = "boreal forest",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_TEMPERATE_GRASSLAND] = {
		.name = "temperate grassland",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_WOODLAND] = {
		.name = "woodland",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_TEMPERATE_SEASONAL_FOREST] = {
		.name = "temperate seasonal forest",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_TEMPERATE_RAINFOREST] = {
		.name = "temperate rainforest",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_DESERT] = {
		.name = "desert",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = YELLOW,
                } },
	},

	[BIOME_SAVANNAH] = {
		.name = "savannah",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = YELLOW,
                } },
	},

	[BIOME_TROPICAL_SEASONAL_FOREST] = {
		.name = "tropical seasonal forest",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = GREEN,
                } },
	},

	[BIOME_TROPICAL_RAINFOREST] = {
		.name = "tropical rainforest",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = BLACK,
                } },
	},

	[BIOME_VOLCANIC] = {
		.name = "volcanic",
                .type = STYPE_BIOME,
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
