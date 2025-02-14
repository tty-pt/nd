#include <xxhash.h>

#include "noise.h"
#include "../../include/params.h"

enum base_plant {
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
        .type = STYPE_CONSUMABLE,
        .sp = { .consumable = { .food = 3 } },
};

DROP carrot_drop = {
        .y = 0,
};

SKEL stick = {
        .name = "stick",
        .type = STYPE_OTHER,
};

DROP stick_drop = {
        .y = 0,
        .yield = 1,
        .yield_v = 0x3,
};

SKEL tomato = {
        .name = "tomato",
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
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_BOLD ANSI_FG_GREEN, 't', 'T', ANSI_RESET_BOLD,
		32, 100, 180, 350,
		1
	} },
}, {
	// woodland / grassland / shrubland
	.name = "betula pendula",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_YELLOW, 'x', 'X', "",
		30, 86, 0, 341,
		4,
	} },
}, {
	.name = "linum usitatissimum",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_YELLOW, 'x', 'X', "",
		30, 86, 20, 341,
		.y = 30,
	} },
}, {
	// woodland / grassland?
	.name = "betula pubescens",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_WHITE, 'x', 'X', "",
		50, 146, 500, 900,
		4
	} },
}, {
	// temperate forest
	.name = "abies alba",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_BOLD ANSI_FG_GREEN, 'a', 'A', ANSI_RESET_BOLD,
		-40, 86, 100, 200,
		4,
	} },
}, {
	// desert
	.name = "arthrocereus rondonianus",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_BOLD ANSI_FG_GREEN, 'i', 'I', "",
		110, 190, 10, 180,
		4,
	} },
}, {
	// savannah
	.name = "acacia senegal",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_BOLD ANSI_FG_GREEN, 't', 'T', "",
		40, 150, 20, 345,
		4,
	} },
}, {
	.name = "daucus carota",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_WHITE, 'x', 'X', "",
		38, 96, 100, 200,
		4
	} },
}, {
	.name = "solanum lycopersicum",
	.type = STYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_RED, 'x', 'X', "", 
		50, 98, 100, 200,
		4
	} },
}};

unsigned plant_refs[PLANT_MAX];

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
spawn(unsigned where_ref, void *arg, pos_t pos)
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

void init(void) {
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
