#include "biome.h"
#include "params.h"
#include "externs.h"
#include "mdb.h"

struct object_skeleton biomes[] = {
	[BIOME_WATER] = {
                .name = "water",
                .description = "",
                .art = "water.jpg",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_BLUE,
                } },
	},

	[BIOME_PERMANENT_ICE] = {
		.name = "permanent ice",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_WHITE,
                } },
	},

	[BIOME_TUNDRA] = {
		.name = "tundra",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},
	[BIOME_TUNDRA2] = {
		.name = "tundra2",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},
	[BIOME_TUNDRA3] = {
		.name = "tundra3",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},
	[BIOME_TUNDRA4] = {
		.name = "tundra4",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},

	[BIOME_COLD_DESERT] = {
		.name = "cold desert",
                .description = "",
                .art = "cold_desert.jpg",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_CYAN,
                } },
	},
	[BIOME_SHRUBLAND] = {
		.name = "shrubland",
                .description = "",
                .art = "shrubland.jpg",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},
	[BIOME_CONIFEROUS_FOREST] = {
		.name = "coniferous forest",
                .description = "",
                .art = "coniferous_forest.jpg",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},
	[BIOME_BOREAL_FOREST] = {
		.name = "boreal forest",
                .description = "",
                .art = "boreal_forest.jpg",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_TEMPERATE_GRASSLAND] = {
		.name = "temperate grassland",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},
	[BIOME_WOODLAND] = {
		.name = "woodland",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},
	[BIOME_TEMPERATE_SEASONAL_FOREST] = {
		.name = "temperate seasonal forest",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},
	[BIOME_TEMPERATE_RAINFOREST] = {
		.name = "temperate rainforest",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},

	[BIOME_DESERT] = {
		.name = "desert",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_YELLOW,
                } },
	},
	[BIOME_SAVANNAH] = {
		.name = "savannah",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_YELLOW,
                } },
	},
	[BIOME_TROPICAL_SEASONAL_FOREST] = {
		.name = "tropical seasonal forest",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_GREEN,
                } },
	},
	[BIOME_TROPICAL_RAINFOREST] = {
		.name = "tropical rainforest",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_BLACK,
                } },
	},

	[BIOME_VOLCANIC] = {
		.name = "volcanic",
                .description = "",
                .art = "",
                .avatar = "",
                .type = S_TYPE_BIOME,
                .sp = { .biome = {
                        .bg = ANSI_BG_BLACK,
                } },
	},
};
