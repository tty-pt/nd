#include "uapi/skel.h"
#include "plant.h"
#include "params.h"

LHASH_DEF(skel, SKEL);
LHASH_DEF(drop, DROP);
LHASH_ASSOC_DEF(adrop, skel, drop);

LHASH_DEF(element, element_t);

element_t element_map[] = {
	[ELM_PHYSICAL] = {
		.color = "",
		.weakness = ELM_VAMP,
	},
	[ELM_FIRE] = {
		.color = ANSI_FG_RED,
		.weakness = ELM_ICE,
	},
	[ELM_ICE] = {
		.color = ANSI_FG_BLUE,
		.weakness = ELM_FIRE,
	},
	[ELM_AIR] = {
		.color = ANSI_FG_WHITE,
		.weakness = ELM_DARK,
	},
	[ELM_EARTH] = {
		.color = ANSI_FG_YELLOW,
		.weakness = ELM_AIR,
	},
	[ELM_SPIRIT] = {
		.color = ANSI_FG_MAGENTA,
		.weakness = ELM_VAMP,
	},
	[ELM_VAMP] = {
		.color = ANSI_BOLD ANSI_FG_BLACK,
		.weakness = ELM_SPIRIT,
	},
	[ELM_DARK] = {
		.color = ANSI_BOLD ANSI_FG_BLACK,
		.weakness = ELM_EARTH,
	},
};

static inline void elements_init() {
	element_lhash = lhash_init();
	for (unsigned i = 0; i < ELM_MAX; i++)
		element_new(&element_map[i]);
}

void biomes_init();
void plants_init();
void spells_init();
void mobs_init();

void skel_init() {
	skel_lhash = lhash_init();
	drop_lhash = lhash_init();
	adrop_ahd = hash_cinit(NULL, NULL, 0644, QH_DUP);
	elements_init();
	biomes_init();
	plants_init();
	spells_init();
	mobs_init();
}
