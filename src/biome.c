#include "biome.h"
#include "params.h"
#ifndef CLIENT
#include "externs.h"
#include "db.h"
#endif

biome_t biomes[] = {
	[BIOME_WATER] = {
		.bg = ANSI_BG_BLUE,
	},

	[BIOME_COLD_DRY] = {
		.bg = ANSI_BG_BLUE,
	},
	[BIOME_COLD] = {
		.bg = ANSI_BG_CYAN,
	},
	[BIOME_COLD_WET] = {
		.bg = ANSI_BG_WHITE,
	},

	[BIOME_TUNDRA] = {
		.bg = ANSI_BG_CYAN,
	},
	[BIOME_TAIGA] = {
		.bg = ANSI_BG_GREEN,
	},
	[BIOME_RAIN_FOREST] = {
		.bg = ANSI_BG_GREEN,
	},

	[BIOME_SHRUBLAND] = {
		.bg = ANSI_BG_GREEN,
	},
	[BIOME_WOODLAND] = {
		.bg = ANSI_BG_GREEN,
	},
	[BIOME_FOREST] = {
		.bg = ANSI_BG_GREEN,
	},

	[BIOME_DESERT] = {
		.bg = ANSI_BG_YELLOW,
	},
	[BIOME_SAVANNAH] = {
		.bg = ANSI_BG_YELLOW,
	},
	[BIOME_SEASONAL_FOREST] = {
		.bg = ANSI_RESET,
	},

	[BIOME_VOLCANIC] = {
		.bg = ANSI_RESET,
	},
};
