#ifndef SPELL_H
#define SPELL_H

#include "object.h"
#include "attr.h"
#include "command.h"

#define ELEMENT(idx) (&element_map[idx])

typedef struct {
	char *color;
	enum element weakness;
} element_t;

extern element_t element_map[];

#define DEBUF_DURATION(ra) 20 * (RARE_MAX - ra) / RARE_MAX
#define DEBUF_DMG(sp_dmg, duration) ((long) 2 * sp_dmg) / duration
#define DEBUF_TYPE_MASK 0xf
#define DEBUF_TYPE(sp) (sp->flags & DEBUF_TYPE_MASK)

#define SPELL_SKELETON(idx) (&spell_skeleton_map[idx])
#define SPELL_COST(dmg, y, no_bdmg) (no_bdmg ? 0 : dmg) + dmg / (1 << y)

enum spell_type {
	SPELL_HEAL,
	SPELL_FOCUS,
	SPELL_FIRE_FOCUS,
	SPELL_CUT,
	SPELL_FIREBALL,
	SPELL_WEAKEN,
	SPELL_DISTRACT,
	SPELL_FREEZE,
	SPELL_LAVA_SHIELD,
	SPELL_WIND_VEIL,
	SPELL_STONE_SKIN,
};

void spells_init(struct spell sps[8], OBJ *player);
void debuf_end(ENT *player, unsigned i);
void debufs_end(ENT *player);
int debufs_process(OBJ *player);
void debuf_notify(OBJ *player, struct debuf *d, short val);
int spell_cast(OBJ *player, OBJ *target, unsigned slot);
int spells_cast(OBJ *player, OBJ *target);

#endif
