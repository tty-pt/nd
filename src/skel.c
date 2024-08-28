#include "uapi/skel.h"
#include "plant.h"
#include "mob.h"

LHASH_DEF(skel, SKEL);
LHASH_DEF(drop, DROP);
LHASH_ASSOC_DEF(adrop, skel, drop);

void skel_init() {
	skel_lhash = lhash_init();
	drop_lhash = lhash_init();
	adrop_ahd = hash_cinit(NULL, NULL, 0644, QH_DUP);
	biomes_init();
	plants_init();
	spells_init();
	mobs_init();
}
