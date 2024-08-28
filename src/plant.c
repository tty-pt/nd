#include "noise.h"

#include <string.h>
#include <xxhash.h>

#include "params.h"
#include "debug.h"

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
		/* .drop = { &stick_drop, NULL }, */
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
		/* .drop = { &stick_drop, NULL }, */
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
		/* .drop = { &stick_drop, NULL }, */
		4,
	} },
}, {
	.name = "linum usitatissimum",
	.description = "",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_YELLOW, 'x', 'X', "",
		30, 86, 20, 341,
		/* .drop = { &stick_drop, NULL }, */
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
		/* .drop = { &stick_drop, NULL }, */
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
		/* .drop = { &stick_drop, NULL }, */
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
		/* { NULL }, */
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
		/* .drop = { &stick_drop, NULL }, */
		4,
	} },
}, {
	.name = "daucus carota",
	.description = "",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_WHITE, 'x', 'X', "",
		38, 96, 100, 200,
		/* { &carrot_drop, NULL }, */
		4
	} },
}, {
	.name = "solanum lycopersicum",
	.description = "",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_RED, 'x', 'X', "", 
		50, 98, 100, 200,
		/* { "tomato", "", "" }, 1, */
		/* { NULL }, */
		4
	} },
}};

unsigned plant_refs[PLANT_MAX];

static inline int
plant_noise(unsigned char *plid, coord_t tmp, ucoord_t rn, noise_t v, unsigned n)
{
	SKEL skel = skel_get(plant_refs[n]);
	SPLA *pl = &skel.sp.plant;

        /* warn("plant_noise %s y: %u v: %u O: %u\n", obj_skel->name, pl->y, v, NOISE_MAX >> pl->y); */
        if (v >= (NOISE_MAX >> pl->y))
                return 0;
	/* if (((v >> 6) ^ (v >> 3) ^ v) & 1) */
	/* 	return 0; */

	if (tmp < pl->tmp_max && tmp > pl->tmp_min
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

                cpln = plant_noise(idc, tmp, rn, v, i);
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
_plants_add(dbref where_ref, struct bio *bio, pos_t pos)
{
	register int i, n;
	noise_t v = XXH32((const char *) pos, sizeof(pos_t), 1);

        for (i = 0; i < 3; i++, v >>= 4) {
                n = PLANT_N(bio->pd.n, i);

		if (!n)
			continue;

		OBJ plant;
		dbref plant_ref = object_add(&plant, plant_refs[bio->pd.id[i]], where_ref, &v);
		PLA *pplant = &plant.sp.plant;
		pplant->plid = bio->pd.id[i];
		pplant->size = n;
		obj_set(plant_ref, &plant);
        }
}

void
plants_add(dbref where_ref, void *arg, pos_t pos)
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

void plants_init() {
	unsigned carrot_ref = skel_new(&carrot);
	carrot_drop.skel = carrot_ref;
	unsigned carrot_drop_ref = drop_new(&carrot_drop);

	unsigned stick_ref = skel_new(&carrot);
	stick_drop.skel = stick_ref;
	unsigned stick_drop_ref = drop_new(&stick_drop);

	unsigned tomato_ref = skel_new(&tomato);
	tomato_drop.skel = tomato_ref;
	unsigned tomato_drop_ref = drop_new(&tomato_drop);

	for (unsigned i = 0; i < PLANT_MAX; i++)
		plant_refs[i] = skel_new(&plants_map[i]);

	adrop_add(plant_refs[PLANT_PINUS_SILVESTRIS], stick_drop_ref);
	adrop_add(plant_refs[PLANT_PSEUDOTSUGA_MENZIESII], stick_drop_ref);
	adrop_add(plant_refs[PLANT_BETULA_PENDULA], stick_drop_ref);
	adrop_add(plant_refs[PLANT_LINUM_USITATISSIMUM], stick_drop_ref);
	adrop_add(plant_refs[PLANT_BETULA_PUBESCENS], stick_drop_ref);
	adrop_add(plant_refs[PLANT_ABIES_ALBA], stick_drop_ref);
	adrop_add(plant_refs[PLANT_ARTHROCEREUS_RONDONIANUS], stick_drop_ref);
	adrop_add(plant_refs[PLANT_ACACIA_SENEGAL], stick_drop_ref);
	adrop_add(plant_refs[PLANT_DAUCUS_CAROTA], carrot_drop_ref);
	adrop_add(plant_refs[PLANT_SOLANUM_LYCOPERSICUM], tomato_drop_ref);
}
