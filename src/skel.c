#include "uapi/skel.h"
#include "plant.h"
#include "params.h"

unsigned skel_hd, drop_hd, adrop_hd, element_hd;

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
	element_hd = lhash_init(sizeof(element_t));
	for (unsigned i = 0; i < ELM_MAX; i++)
		lhash_new(element_hd, &element_map[i]);
}

void biomes_init();
void plants_init();
void spells_init();
void mobs_init();

void skel_init() {
	skel_hd = lhash_init(sizeof(SKEL));
	drop_hd = lhash_init(sizeof(DROP));
	adrop_hd = ahash_init();
	elements_init();
	biomes_init();
	plants_init();
	spells_init();
	mobs_init();
}
