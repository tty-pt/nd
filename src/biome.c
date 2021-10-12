#include "biome.h"
#include "params.h"
#ifndef CLIENT
#include "externs.h"
#include "mdb.h"
#endif

biome_t biomes[] = {
	[BIOME_WATER] = {
		.bg = ANSI_BG_BLUE,
		.name = "water",
	},

	[BIOME_PERMANENT_ICE] = {
		.bg = ANSI_BG_WHITE,
		.name = "permanent ice",
	},

	[BIOME_TUNDRA] = {
		.bg = ANSI_BG_CYAN,
		.name = "tundra",
	},
	[BIOME_TUNDRA2] = {
		.bg = ANSI_BG_CYAN,
		.name = "tundra2",
	},
	[BIOME_TUNDRA3] = {
		.bg = ANSI_BG_CYAN,
		.name = "tundra3",
	},
	[BIOME_TUNDRA4] = {
		.bg = ANSI_BG_CYAN,
		.name = "tundra4",
	},

	[BIOME_COLD_DESERT] = {
		.bg = ANSI_BG_CYAN,
		.name = "cold desert",
	},
	[BIOME_SHRUBLAND] = {
		.bg = ANSI_BG_GREEN,
		.name = "shrubland",
	},
	[BIOME_CONIFEROUS_FOREST] = {
		.bg = ANSI_BG_GREEN,
		.name = "coniferous forest",
	},
	[BIOME_BOREAL_FOREST] = {
		.bg = ANSI_BG_GREEN,
		.name = "boreal forest",
	},

	[BIOME_TEMPERATE_GRASSLAND] = {
		.bg = ANSI_BG_GREEN,
		.name = "temperate grassland",
	},
	[BIOME_WOODLAND] = {
		.bg = ANSI_BG_GREEN,
		.name = "woodland",
	},
	[BIOME_TEMPERATE_SEASONAL_FOREST] = {
		.bg = ANSI_BG_GREEN,
		.name = "temperate seasonal forest",
	},
	[BIOME_TEMPERATE_RAINFOREST] = {
		.bg = ANSI_BG_GREEN,
		.name = "temperate rainforest",
	},

	[BIOME_DESERT] = {
		.bg = ANSI_BG_YELLOW,
		.name = "desert",
	},
	[BIOME_SAVANNAH] = {
		.bg = ANSI_BG_YELLOW,
		.name = "savannah",
	},
	[BIOME_TROPICAL_SEASONAL_FOREST] = {
		.bg = ANSI_BG_GREEN,
		.name = "tropical seasonal forest",
	},
	[BIOME_TROPICAL_RAINFOREST] = {
		.bg = ANSI_BG_BLACK,
		.name = "tropical rainforest",
	},

	[BIOME_VOLCANIC] = {
		.bg = ANSI_BG_BLACK,
		.name = "volcanic",
	},
};
