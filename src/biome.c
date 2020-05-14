#include "biome.h"
#include "params.h"
#include "debug.h"
#ifndef WEB_CLIENT
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

/* TODO calculate water needs */
struct plant plants[] = {{
	// taiga
	{ "pinus silvestris", "", "" }, ANSI_BOLD ANSI_FG_GREEN, 'x', 'X', ANSI_RESET_BOLD,
	-65, 70, 50, 200, 1,
	{ "fruit", "", "" }, 1,
}, {	// temperate rainforest
	{ "pseudotsuga menziesii", "", "" }, ANSI_BOLD ANSI_FG_GREEN, 't', 'T', ANSI_RESET_BOLD,
	32, 100, 180, 350, 1,
	{ "fruit", "", "" }, 1,
}, {	// woodland / grassland / shrubland
	{ "betula pendula", "", "" }, ANSI_FG_YELLOW, 'x', 'X', "",
	-4, 86, 400, 700, 1,
	{ "fruit", "", "" }, 1,
}, {	// woodland / grassland?
	{ "betula pubescens", "", "" }, ANSI_FG_WHITE, 'x', 'X', "",
	0, 96, 500, 900, 1,
	{ "fruit", "", "" }, 1,
}, {	// temperate forest
	{ "abies alba", "", "" }, ANSI_BOLD ANSI_FG_GREEN, 'x', 'X', ANSI_RESET_BOLD,
	-40, 86, 100, 200, 1,
	{ "fruit", "", "" }, 1,
}, {	// desert
	{ "arthrocereus rondonianus", "", "" }, ANSI_BOLD ANSI_FG_GREEN, 'i', 'I', "",
	110, 130, 10, 150, 1,
	{ "fruit", "", "" }, 1,
}, {	// savannah
	{ "acacia senegal", "", "" }, ANSI_BOLD ANSI_FG_GREEN, 't', 'T', "",
	40, 120, 20, 120, 1,
	{ "fruit", "", "" }, 1,
}, {
	{ "daucus carota", "", "" }, ANSI_FG_WHITE, 'x', 'X', "",
	38, 96, 100, 200, 1,
	{ "carrot", "", "" }, 1,
}, {
	{ "solanum lycopersicum", "", "" }, ANSI_FG_RED, 'x', 'X', "", 
	50, 98, 100, 200, 5,
	{ "tomato", "", "" }, 1,
}};

static const unsigned char plid_max = sizeof(plants) / sizeof(struct plant);

static inline int
plant_noise(unsigned char *plid, coord_t tmp, ucoord_t rn, morton_t v, unsigned char n)
{
	struct plant *pl = &plants[n];

	CBUG(n >= plid_max);

	if (((v >> 6) ^ (v >> 3) ^ v) & 1)
		return 0;

	if (v & 0x18
	    && tmp < pl->tmp_max && tmp > pl->tmp_min
	    && rn < pl->rn_max && rn > pl->rn_min) {
		*plid = n;
		v = (v >> 1) & PLANT_MASK;
		if (v == 3)
			v = 0;
                return v;
	}

	return 0;
}

void
plants_noise(struct plant_data *pd, morton_t ty, coord_t tmp, ucoord_t rn, unsigned n)
{
	morton_t v = ty;
	register int i, cpln;
	unsigned char *idc = pd->id;

	memset(pd->id, 0, n);
	pd->n = 0;

	for (i = pd->max;
	     i < plid_max && idc < pd->id + n;
	     i++, v >>= 8) {
		if (!v)
			v = ~ty >> 4;

                cpln = plant_noise(idc, tmp, rn, v, i);
		if (cpln) {
			idc++;
                        pd->n |= cpln << (i * 2);
		}
	}

	pd->max = i;
}

void
plants_shuffle(struct plant_data *pd, morton_t v)
{
        unsigned char apln[3] = {
                PLANT_N(pd->n, 0),
                PLANT_N(pd->n, 1),
                PLANT_N(pd->n, 2)
        };
	register unsigned char i, aux;

        for (i = 1; i < 3; i++) {
                if (!(v & i))
                        continue;
                aux = pd->id[i - 1];
                pd->id[i - 1] = pd->id[i];
                pd->id[i] = aux;

                aux = apln[i - 1];
                apln[i - 1] = apln[i];
                apln[i] = aux;
        }

	pd->n = apln[0]
		| (apln[1] << 2)
		| (apln[2] << 4);
}

static inline unsigned
plant_yield(struct plant *pl, coord_t tmp,
		unsigned char n)
{
	register unsigned ptmin = pl->tmp_min;
	return random() % (1 + n * (tmp - ptmin) * pl->yield / (pl->tmp_max - ptmin));
}

static inline void
plant_add(int descr, dbref player, dbref where, unsigned char plid, unsigned char n, coord_t tmp)
{
	if (n == 0)
		return;
	struct plant *pl = &plants[plid];
	dbref plant = obj_stack_add(pl->o, where, n);
	dbref fruit = obj_add(pl->fruit, plant);
	struct boolexp *key = parse_boolexp(descr, player, NAME(player), 0);
	unsigned yield = plant_yield(pl, tmp, n);
	SETCONLOCK(plant, key);
        SETPLID(plant, plid);
	SETFOOD(fruit, 3); // TODO
	if (yield > 1)
		SETSTACK(fruit, yield);
}

static inline void
_plants_add(int descr, dbref player, dbref where,
		struct plant_data *pd,
		coord_t tmp)
{
	register int i, aux;

        for (i = 0; i < 3; i++) {
                aux = PLANT_N(pd->n, i);

		if (!aux)
			continue;

		plant_add(descr, player, where,
				pd->id[i], aux, tmp);
        }
}

void
plants_add(int descr, dbref player, dbref where,
		struct plant_data *pd,
		morton_t ty, coord_t tmp,
		ucoord_t rn)
{
	struct plant_data epd;

        if (pd->n)
                _plants_add(descr, player, where,
				pd, tmp);

	plants_noise(&epd, ty, tmp, rn, PLANT_EXTRA);

        if (epd.n)
                _plants_add(descr, player, where,
				&epd, tmp);
}
