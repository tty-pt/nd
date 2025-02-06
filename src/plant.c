#include <xxhash.h>

#include "noise.h"
#include "params.h"

enum plant {
	PLANT_PINUS_SILVESTRIS,
	PLANT_PSEUDOTSUGA_MENZIESII,
	PLANT_BETULA_PENDULA,
	PLANT_LINUM_USITATISSIMUM,
	PLANT_BETULA_PUBESCENS,
	PLANT_ABIES_ALBA,
	PLANT_ARTHROCEREUS_RONDONIANUS,
	PLANT_ACACIA_SENEGAL,
	PLANT_DAUCUS_CAROTA,
	PLANT_SOLANUM_LYCOPERSICUM,
	PLANT_MAX,
};

SKEL carrot = {
        .name = "carrot",
        .description = "",
        .type = STYPE_CONSUMABLE,
        .sp = { .consumable = { .food = 3 } },
};

DROP carrot_drop = {
        .y = 0,
};

SKEL stick = {
        .name = "stick",
        .description = "",
        .type = STYPE_OTHER,
};

DROP stick_drop = {
        .y = 0,
        .yield = 1,
        .yield_v = 0x3,
};

SKEL tomato = {
        .name = "tomato",
        .description = "",
        .type = STYPE_CONSUMABLE,
        .sp = { .consumable = { .food = 4 } },
};

DROP tomato_drop = {
        .y = 3,
};

/* TODO calculate water needs */
SKEL plants_map[] = {{
	// taiga
	.name = "pinus sylvestris",
	.description = "",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		.pre = ANSI_BOLD ANSI_FG_GREEN,
		.small = 'x',
		.big = 'X',
		.post = ANSI_RESET_BOLD,
		.tmp_min = 30,
		.tmp_max = 70,
		.rn_min = 50,
		.rn_max = 1024,
		.y = 4,
	} },
}, {
	// temperate rainforest
	.name = "pseudotsuga menziesii",
	.description = "",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_BOLD ANSI_FG_GREEN, 't', 'T', ANSI_RESET_BOLD,
		32, 100, 180, 350,
		1
	} },
}, {
	// woodland / grassland / shrubland
	.name = "betula pendula",
	.description = "",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_YELLOW, 'x', 'X', "",
		30, 86, 0, 341,
		4,
	} },
}, {
	.name = "linum usitatissimum",
	.description = "",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_YELLOW, 'x', 'X', "",
		30, 86, 20, 341,
		.y = 30,
	} },
}, {
	// woodland / grassland?
	.name = "betula pubescens",
	.description = "", 
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_WHITE, 'x', 'X', "",
		50, 146, 500, 900,
		4
	} },
}, {
	// temperate forest
	.name = "abies alba",
	.description = "",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_BOLD ANSI_FG_GREEN, 'a', 'A', ANSI_RESET_BOLD,
		-40, 86, 100, 200,
		4,
	} },
}, {
	// desert
	.name = "arthrocereus rondonianus",
	.description = "",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_BOLD ANSI_FG_GREEN, 'i', 'I', "",
		110, 190, 10, 180,
		4,
	} },
}, {
	// savannah
	.name = "acacia senegal",
	.description = "",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_BOLD ANSI_FG_GREEN, 't', 'T', "",
		40, 150, 20, 345,
		4,
	} },
}, {
	.name = "daucus carota",
	.description = "",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_WHITE, 'x', 'X', "",
		38, 96, 100, 200,
		4
	} },
}, {
	.name = "solanum lycopersicum",
	.description = "",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_RED, 'x', 'X', "", 
		50, 98, 100, 200,
		4
	} },
}};

unsigned plant_refs[PLANT_MAX];

static inline int
plant_noise(unsigned char *plid, coord_t tmp, ucoord_t rn, noise_t v, unsigned plant_ref)
{
	SKEL skel;
	lhash_get(skel_hd, &skel, plant_ref);
	SPLA *pl = &skel.sp.plant;

        if (v >= (NOISE_MAX >> pl->y))
                return 0;
	/* if (((v >> 6) ^ (v >> 3) ^ v) & 1) */
	/* 	return 0; */

	if (tmp < pl->tmp_max && tmp > pl->tmp_min
	    && rn < pl->rn_max && rn > pl->rn_min) {
		*plid = plant_ref;
		v = (v >> 1) & PLANT_MASK;
		if (v == 3)
			v = 0;
                return v;
	}

	return 0;
}

void
plants_noise(struct plant_data *pd, noise_t ty, coord_t tmp, ucoord_t rn, unsigned n)
{
	noise_t v = ty;
	register int i, cpln;
	unsigned char *idc = pd->id;

	memset(pd->id, 0, n);
	pd->n = 0;

	for (i = pd->max;
	     i < PLANT_MAX && idc < pd->id + n;
	     i++, v >>= 8) {
		if (!v)
			v = XXH32((const char *) &ty, sizeof(ty), i);

		cpln = plant_noise(idc, tmp, rn, v, plant_refs[i]);
		if (cpln) {
                        pd->n |= cpln << ((idc - pd->id) * 2);
			idc++;
		}
	}

	pd->max = *idc;
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

static inline void
_plants_add(unsigned where_ref, struct bio *bio, pos_t pos)
{
	register int i, n;
	noise_t v = XXH32((const char *) pos, sizeof(pos_t), 1);

        for (i = 0; i < 3; i++, v >>= 4) {
                n = PLANT_N(bio->pd.n, i);

		if (!n)
			continue;

		OBJ plant;
		unsigned plant_ref = object_add(&plant, bio->pd.id[i], where_ref, &v);
		PLA *pplant = &plant.sp.plant;
		pplant->plid = bio->pd.id[i];
		pplant->size = n;
		lhash_put(obj_hd, plant_ref, &plant);
        }
}

void
plants_add(unsigned where_ref, void *arg, pos_t pos)
{
	struct bio *bio = arg;
	/* &bio->pd, bio->ty, */
	/* bio->tmp, bio->rn); */
	/* struct plant_data epd; */

        if (bio->pd.n)
                _plants_add(where_ref, bio, pos);

	/* plants_noise(&epd, ty, tmp, rn, PLANT_EXTRA); */

        /* if (epd.n) */
                /* _plants_add(where, &epd, tmp); */
}

void plants_init(void) {
	unsigned carrot_ref = lhash_new(skel_hd, &carrot);
	carrot_drop.skel = carrot_ref;
	unsigned carrot_drop_ref = lhash_new(drop_hd, &carrot_drop);

	unsigned stick_ref = lhash_new(skel_hd, &stick);
	stick_drop.skel = stick_ref;
	unsigned stick_drop_ref = lhash_new(drop_hd, &stick_drop);

	unsigned tomato_ref = lhash_new(skel_hd, &tomato);
	tomato_drop.skel = tomato_ref;
	unsigned tomato_drop_ref = lhash_new(drop_hd, &tomato_drop);

	for (unsigned i = 0; i < PLANT_MAX; i++)
		plant_refs[i] = lhash_new(skel_hd, &plants_map[i]);

	ahash_add(adrop_hd, plant_refs[PLANT_PINUS_SILVESTRIS], stick_drop_ref);
	ahash_add(adrop_hd, plant_refs[PLANT_PSEUDOTSUGA_MENZIESII], stick_drop_ref);
	ahash_add(adrop_hd, plant_refs[PLANT_BETULA_PENDULA], stick_drop_ref);
	ahash_add(adrop_hd, plant_refs[PLANT_LINUM_USITATISSIMUM], stick_drop_ref);
	ahash_add(adrop_hd, plant_refs[PLANT_BETULA_PUBESCENS], stick_drop_ref);
	ahash_add(adrop_hd, plant_refs[PLANT_ABIES_ALBA], stick_drop_ref);
	ahash_add(adrop_hd, plant_refs[PLANT_ARTHROCEREUS_RONDONIANUS], stick_drop_ref);
	ahash_add(adrop_hd, plant_refs[PLANT_ACACIA_SENEGAL], stick_drop_ref);
	ahash_add(adrop_hd, plant_refs[PLANT_DAUCUS_CAROTA], carrot_drop_ref);
	ahash_add(adrop_hd, plant_refs[PLANT_SOLANUM_LYCOPERSICUM], tomato_drop_ref);
}
