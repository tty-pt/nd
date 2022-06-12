#ifndef ENTITY_H
#define ENTITY_H

#include "object.h"
#include "math.h"

#define EQUIP(ent, y) ent->equipment[y]
#define EFFECT(mob, w) mob->e[AF_ ## w]

#define ELEMENT_NEXT(ent, type) \
	element_next(ent, EFFECT(ent, type).mask)

#define G(x) xsqrtx(x)

#define HP_G(v) 10 * G(v)
#define HP_MAX(ent) HP_G(ent->attr[ATTR_CON])

#define MP_G(v) HP_G(v)
#define MP_MAX(ent) MP_G(ent->attr[ATTR_WIZ])

#define SPELL_G(v) G(v)
#define SPELL_DMG(p, sp) SPELL_G(p->attr[ATTR_INT]) + HS(sp)

#define DMG_G(v) G(v)
#define DODGE_G(v) G(v)

#define DMG_BASE(p) DMG_G(p->attr[ATTR_STR])
#define DODGE_BASE(p) DODGE_G(p->attr[ATTR_DEX])

#define EQL(x)		(x & 15)

void recycle(OBJ *player, OBJ *thing);
void enter(OBJ *player, OBJ *loc);

const char * unparse(OBJ *player, OBJ *loc);

int stand_silent(OBJ *player);

int cando(OBJ *player, OBJ *thing, const char *default_fail_msg);

void look_around(OBJ *player);

int equip_affect(ENT *ewho, EQU *equ);
dbref unequip(OBJ *player, unsigned eql);
int equip(OBJ *player, OBJ *eq);

enum element element_next(ENT *ref, register unsigned char a);

static inline unsigned
xsqrtx(unsigned x)
{
	// y * x ^ 3/2
	return x * sqrt(x);
}

void stats_init(ENT *enu, SENT *sk);
void entity_update(OBJ *player);
int entity_damage(OBJ *player, OBJ *target, short amt);

#endif
