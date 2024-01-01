#include "noise.h"

#include <string.h>
#include <xxhash.h>

#include "params.h"
#include "debug.h"

struct object_skeleton carrot = {
        .name = "carrot",
        .description = "",
        .type = S_TYPE_CONSUMABLE,
        .sp = { .consumable = { .food = 3 } },
};

struct drop carrot_drop = {
        .i = &carrot,
        .y = 0,
};

struct object_skeleton stick = {
        .name = "stick",
        .description = "",
        .type = S_TYPE_OTHER,
};

struct drop stick_drop = {
        .i = &stick,
        .y = 0,
        .yield = 1,
        .yield_v = 0x3,
};

/* TODO calculate water needs */
struct object_skeleton plant_skeleton_map[] = {{
	// taiga
	.name = "pinus sylvestris",
	.description = "",
	.type = S_TYPE_PLANT,
	.sp = { .plant = {
		.pre = ANSI_BOLD ANSI_FG_GREEN,
		.small = 'x',
		.big = 'X',
		.post = ANSI_RESET_BOLD,
		.tmp_min = 30,
		.tmp_max = 70,
		.rn_min = 50,
		.rn_max = 1024,
		.drop = { &stick_drop, NULL },
		.y = 4,
	} },
}, {	// temperate rainforest
	.name = "pseudotsuga menziesii",
	.description = "",
	.type = S_TYPE_PLANT,
	.sp = { .plant = {
		ANSI_BOLD ANSI_FG_GREEN, 't', 'T', ANSI_RESET_BOLD,
		32, 100, 180, 350,
		.drop = { &stick_drop, NULL },
		1
	} },
}, {	// woodland / grassland / shrubland
	.name = "betula pendula",
	.description = "",
	.type = S_TYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_YELLOW, 'x', 'X', "",
		30, 86, 0, 341,
		.drop = { &stick_drop, NULL },
		4,
	} },
}, {
	.name = "linum usitatissimum",
	.description = "",
	.type = S_TYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_YELLOW, 'x', 'X', "",
		30, 86, 20, 341,
		.drop = { &stick_drop, NULL },
		.y = 30,
	} },
}, {	// woodland / grassland?
	.name = "betula pubescens",
	.description = "", 
	.type = S_TYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_WHITE, 'x', 'X', "",
		50, 146, 500, 900,
		.drop = { &stick_drop, NULL },
		4
	} },
}, {	// temperate forest
	.name = "abies alba",
	.description = "",
	.type = S_TYPE_PLANT,
	.sp = { .plant = {
		ANSI_BOLD ANSI_FG_GREEN, 'a', 'A', ANSI_RESET_BOLD,
		-40, 86, 100, 200,
		.drop = { &stick_drop, NULL },
		4,
	} },
}, {	// desert
	.name = "arthrocereus rondonianus",
	.description = "",
	.type = S_TYPE_PLANT,
	.sp = { .plant = {
		ANSI_BOLD ANSI_FG_GREEN, 'i', 'I', "",
		110, 190, 10, 180,
		{ NULL }, 4,
	} },
}, {	// savannah
	.name = "acacia senegal",
	.description = "",
	.type = S_TYPE_PLANT,
	.sp = { .plant = {
		ANSI_BOLD ANSI_FG_GREEN, 't', 'T', "",
		40, 150, 20, 345,
		.drop = { &stick_drop, NULL },
		4,
	} },
}, {
	.name = "daucus carota",
	.description = "",
	.type = S_TYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_WHITE, 'x', 'X', "",
		38, 96, 100, 200,
		{ &carrot_drop, NULL }, 4
	} },
}, {
	.name = "solanum lycopersicum",
	.description = "",
	.type = S_TYPE_PLANT,
	.sp = { .plant = {
		ANSI_FG_RED, 'x', 'X', "", 
		50, 98, 100, 200,
		/* { "tomato", "", "" }, 1, */
		{ NULL }, 4
	} },
}};

static inline int
plant_noise(unsigned char *plid, coord_t tmp, ucoord_t rn, noise_t v, unsigned char n)
{
	struct object_skeleton *obj_skel = PLANT_SKELETON(n);
	struct plant_skeleton *pl = &obj_skel->sp.plant;

	CBUG(n >= PLANT_MAX);

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

static inline void
_plants_add(OBJ *where, struct bio *bio, pos_t pos)
{
	register int i, n;
	noise_t v = XXH32((const char *) pos, sizeof(pos_t), 1);

        for (i = 0; i < 3; i++, v >>= 4) {
                n = PLANT_N(bio->pd.n, i);

		if (!n)
			continue;

                struct object_skeleton *obj_skel = PLANT_SKELETON(bio->pd.id[i]);
                OBJ *plant = object_add(obj_skel, where, &v);
		PLA *pplant = &plant->sp.plant;
		pplant->plid = bio->pd.id[i];
		pplant->size = n;
        }
}

void
plants_add(OBJ *where, void *arg, pos_t pos)
{
	struct bio *bio = arg;
	/* &bio->pd, bio->ty, */
	/* bio->tmp, bio->rn); */
	/* struct plant_data epd; */

        if (bio->pd.n)
                _plants_add(where, bio, pos);

	/* plants_noise(&epd, ty, tmp, rn, PLANT_EXTRA); */

        /* if (epd.n) */
                /* _plants_add(where, &epd, tmp); */
}
