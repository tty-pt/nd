#include <nd/nd.h>
#include <string.h>
#include "../bdrink/include/drink.h"

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
        .type = 0, // type later
};

CON carrot_con = { .food = 3 };

DROP carrot_drop = {
        .y = 0,
};

SKEL stick = {
        .name = "stick",
        .type = TYPE_THING,
};

DROP stick_drop = {
        .y = 0,
        .yield = 1,
        .yield_v = 0x3,
};

SKEL tomato = {
        .name = "tomato",
        .type = 0, // type later
};

CON tomato_con = { .food = 4 };

DROP tomato_drop = {
        .y = 3,
};

/* TODO calculate water needs */
SKEL plants_map[] = {{
	// taiga
	.name = "pinus sylvestris",
	.type = TYPE_PLANT,
	.sp = { .plant = {
		.pi = { .fg = GREEN, .flags = BOLD, },
		'x', 'X', 30, 70, 50, 1024, 4,
	} },
}, {
	// temperate rainforest
	.name = "pseudotsuga menziesii",
	.type = TYPE_PLANT,
	.sp = { .plant = {
		.pi = { .fg = GREEN, .flags = BOLD, },
		't', 'T', 32, 100, 180, 350, 1,
	} },
}, {
	// woodland / grassland / shrubland
	.name = "betula pendula",
	.type = TYPE_PLANT,
	.sp = { .plant = {
		.pi = { .fg = YELLOW, },
		'x', 'X', 30, 86, 0, 341, 4,
	} },
}, {
	.name = "linum usitatissimum",
	.type = TYPE_PLANT,
	.sp = { .plant = {
		.pi = { .fg = YELLOW },
		'x', 'X', 30, 86, 20, 341, 30,
	} },
}, {
	// woodland / grassland?
	.name = "betula pubescens",
	.type = TYPE_PLANT,
	.sp = { .plant = {
		.pi = { .fg = WHITE, },
		'x', 'X', 50, 146, 500, 900, 4,
	} },
}, {
	// temperate forest
	.name = "abies alba",
	.type = TYPE_PLANT,
	.sp = { .plant = {
		.pi = { .fg = GREEN, .flags = BOLD, },
		'a', 'A', -40, 86, 100, 200, 4,
	} },
}, {
	// desert
	.name = "arthrocereus rondonianus",
	.type = TYPE_PLANT,
	.sp = { .plant = {
		.pi = { .fg = GREEN, .flags = BOLD, },
		'i', 'I', 110, 190, 10, 180, 4,
	} },
}, {
	// savannah
	.name = "acacia senegal",
	.type = TYPE_PLANT,
	.sp = { .plant = {
		.pi = { .fg = GREEN, .flags = BOLD, },
		't', 'T', 40, 150, 20, 345, 4,
	} },
}, {
	.name = "daucus carota",
	.type = TYPE_PLANT,
	.sp = { .plant = {
		.pi = { .fg = WHITE, },
		'x', 'X', 38, 96, 100, 200, 4,
	} },
}, {
	.name = "solanum lycopersicum",
	.type = TYPE_PLANT,
	.sp = { .plant = {
		.pi = { .fg = RED },
		'x', 'X', 50, 98, 100, 200, 4,
	} },
}};

static inline void
_plants_add(unsigned where_ref, struct bio *bio, pos_t pos)
{
	register int i, n;
	uint32_t v = XXH32((const char *) pos, sizeof(pos_t), 1);

        for (i = 0; i < 3; i++, v >>= 4) {
                n = PLANT_N(bio->pd.n, i);

		if (!n)
			continue;

		OBJ plant;
		unsigned plant_ref = object_add(&plant, bio->pd.id[i], where_ref, &v);
		PLA *pplant = &plant.sp.plant;
		pplant->plid = bio->pd.id[i];
		pplant->size = n;
		nd_put(HD_OBJ, &plant_ref, &plant);
        }
}

void
mod_spawn(void *arg)
{
	struct spawn_arg *sa = arg;
	struct bio *bio = sa->arg;
	/* &bio->pd, bio->ty, */
	/* bio->tmp, bio->rn); */
	/* struct plant_data epd; */

        if (bio->pd.n)
                _plants_add(sa->where_ref, bio, sa->pos);

	/* plants_noise(&epd, ty, tmp, rn, PLANT_EXTRA); */

        /* if (epd.n) */
                /* _plants_add(where, &epd, tmp); */
}

void mod_install(void *arg __attribute__ ((unused))) {
	unsigned plant_refs[PLANT_MAX];

	memcpy(&carrot.sp.raw, &carrot_con, sizeof(carrot_con));
	memcpy(&tomato.sp.raw, &tomato_con, sizeof(tomato_con));

	unsigned consumable_type;
	nd_get(HD_RTYPE, &consumable_type, "drink");
	tomato.type = carrot.type = consumable_type;

	carrot_drop.skel = nd_put(HD_SKEL, NULL, &carrot);
	unsigned carrot_drop_ref = nd_put(HD_DROP, NULL, &carrot_drop);

	stick_drop.skel = nd_put(HD_SKEL, NULL, &stick);
	unsigned stick_drop_ref = nd_put(HD_DROP, NULL, &stick_drop);

	tomato_drop.skel = nd_put(HD_SKEL, NULL, &tomato);
	unsigned tomato_drop_ref = nd_put(HD_DROP, NULL, &tomato_drop);

	for (unsigned i = 0; i < PLANT_MAX; i++) {
		unsigned ref = nd_put(HD_SKEL, NULL, &plants_map[i]);
		plant_refs[i] = ref;
		nd_put(HD_PLANT, &ref, &ref);
	}

	nd_put(HD_ADROP, &plant_refs[PLANT_PINUS_SILVESTRIS], &stick_drop_ref);
	nd_put(HD_ADROP, &plant_refs[PLANT_PSEUDOTSUGA_MENZIESII], &stick_drop_ref);
	nd_put(HD_ADROP, &plant_refs[PLANT_BETULA_PENDULA], &stick_drop_ref);
	nd_put(HD_ADROP, &plant_refs[PLANT_LINUM_USITATISSIMUM], &stick_drop_ref);
	nd_put(HD_ADROP, &plant_refs[PLANT_BETULA_PUBESCENS], &stick_drop_ref);
	nd_put(HD_ADROP, &plant_refs[PLANT_ABIES_ALBA], &stick_drop_ref);
	nd_put(HD_ADROP, &plant_refs[PLANT_ARTHROCEREUS_RONDONIANUS], &stick_drop_ref);
	nd_put(HD_ADROP, &plant_refs[PLANT_ACACIA_SENEGAL], &stick_drop_ref);
	nd_put(HD_ADROP, &plant_refs[PLANT_DAUCUS_CAROTA], &carrot_drop_ref);
	nd_put(HD_ADROP, &plant_refs[PLANT_SOLANUM_LYCOPERSICUM], &tomato_drop_ref);
}
