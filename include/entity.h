#ifndef ENTITY_H
#define ENTITY_H

#include <math.h>
#include "object.h"
#include "biome.h"

#define EQUIP(ent, y) ent->equipment[y]
#define EFFECT(ent, w) ent->e[AF_ ## w]

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

#define EQT(x)		(x>>6)
#define EQL(x)		(x & 15)
#define EQ(i, t)	(i | (t<<6))

extern struct wts phys_wts[];

static inline unsigned
xsqrtx(unsigned x)
{
	return x * sqrt(x);
}

ENT *birth(OBJ *player);
int kill_dodge(OBJ *player, struct wts wts);
short kill_dmg(enum element dmg_type,
		short dmg, short def,
		enum element def_type);

void recycle(OBJ *player, OBJ *thing);
void enter(OBJ *player, OBJ *loc);

const char * unparse(OBJ *player, OBJ *loc);

void sit(OBJ *player, char const *what);
int stand_silent(OBJ *player);
void stand(OBJ *who);

int cando(OBJ *player, OBJ *thing, const char *default_fail_msg);
int controls(OBJ *who, OBJ *what);
int payfor(OBJ *who, int cost);

void look_around(OBJ *player);

int equip_affect(ENT *ewho, EQU *equ);
int equip(OBJ *player, OBJ *eq);
dbref unequip(OBJ *player, unsigned eql);

enum element element_next(ENT *ref, register unsigned char a);

void stats_init(ENT *enu, SENT *sk);
void entity_update(OBJ *player);
int entity_damage(OBJ *player, OBJ *target, short amt);

void entities_add(OBJ *where, enum biome, long long pdn);

#endif
