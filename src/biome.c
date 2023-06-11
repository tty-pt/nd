#include "biome.h"
#include "params.h"

struct object_skeleton biomes[] = {
	[BIOME_WATER] = {
                .name = "water",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_BLUE,
                } },
	},

	[BIOME_PERMANENT_ICE] = {
		.name = "permanent ice",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_WHITE,
                } },
	},

	[BIOME_TUNDRA] = {
		.name = "tundra",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},

	[BIOME_TUNDRA2] = {
		.name = "tundra2",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},

	[BIOME_TUNDRA3] = {
		.name = "tundra3",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},

	[BIOME_TUNDRA4] = {
		.name = "tundra4",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},

	[BIOME_COLD_DESERT] = {
		.name = "cold desert",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},

	[BIOME_SHRUBLAND] = {
		.name = "shrubland",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_CONIFEROUS_FOREST] = {
		.name = "coniferous forest",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_BOREAL_FOREST] = {
		.name = "boreal forest",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_TEMPERATE_GRASSLAND] = {
		.name = "temperate grassland",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_WOODLAND] = {
		.name = "woodland",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_TEMPERATE_SEASONAL_FOREST] = {
		.name = "temperate seasonal forest",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_TEMPERATE_RAINFOREST] = {
		.name = "temperate rainforest",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_DESERT] = {
		.name = "desert",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_YELLOW,
                } },
	},

	[BIOME_SAVANNAH] = {
		.name = "savannah",
                .description = "a place that is hot and dry",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_YELLOW,
                } },
	},

	[BIOME_TROPICAL_SEASONAL_FOREST] = {
		.name = "tropical seasonal forest",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_TROPICAL_RAINFOREST] = {
		.name = "tropical rainforest",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_BLACK,
                } },
	},

	[BIOME_VOLCANIC] = {
		.name = "volcanic",
                .description = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_BLACK,
                } },
	},
};
