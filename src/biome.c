#include "biome.h"
#include "params.h"

unsigned biome_refs[BIOME_MAX];

SKEL biomes_skel[] = {
	[BIOME_WATER] = {
                .name = "water",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_BLUE,
                } },
	},

	[BIOME_PERMANENT_ICE] = {
		.name = "permanent ice",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_WHITE,
                } },
	},

	[BIOME_TUNDRA] = {
		.name = "tundra",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},

	[BIOME_TUNDRA2] = {
		.name = "tundra2",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},

	[BIOME_TUNDRA3] = {
		.name = "tundra3",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},

	[BIOME_TUNDRA4] = {
		.name = "tundra4",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},

	[BIOME_COLD_DESERT] = {
		.name = "cold desert",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},

	[BIOME_SHRUBLAND] = {
		.name = "shrubland",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_CONIFEROUS_FOREST] = {
		.name = "coniferous forest",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_BOREAL_FOREST] = {
		.name = "boreal forest",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_TEMPERATE_GRASSLAND] = {
		.name = "temperate grassland",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_WOODLAND] = {
		.name = "woodland",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_TEMPERATE_SEASONAL_FOREST] = {
		.name = "temperate seasonal forest",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_TEMPERATE_RAINFOREST] = {
		.name = "temperate rainforest",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_DESERT] = {
		.name = "desert",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_YELLOW,
                } },
	},

	[BIOME_SAVANNAH] = {
		.name = "savannah",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_YELLOW,
                } },
	},

	[BIOME_TROPICAL_SEASONAL_FOREST] = {
		.name = "tropical seasonal forest",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_TROPICAL_RAINFOREST] = {
		.name = "tropical rainforest",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_BLACK,
                } },
	},

	[BIOME_VOLCANIC] = {
		.name = "volcanic",
                .type = STYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_BLACK,
                } },
	},
};

void biomes_init(void) {
	for (unsigned i = 0; i < BIOME_MAX; i++)
		biome_refs[i] = lhash_new(skel_hd, &biomes_skel[i]);
}
