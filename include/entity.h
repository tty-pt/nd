#ifndef ENTITY_H
#define ENTITY_H

#include <math.h>
#include "object.h"
#include "biome.h"

#define EQUIP(ent, y) (ent)->equipment[y]
#define EFFECT(ent, w) (ent)->e[AF_ ## w]

#define ELEMENT_NEXT(ent, type) \
	element_next(ent, EFFECT(ent, type).mask)

#define G(x) xsqrtx(x)

#define HP_G(v) 10 * G(v)
#define HP_MAX(ent) ((unsigned short) HP_G((ent)->attr[ATTR_CON]))

#define MP_G(v) HP_G(v)
#define MP_MAX(ent) ((unsigned short) MP_G((ent)->attr[ATTR_WIZ]))

#define SPELL_G(v) G(v)
#define SPELL_DMG(p, sp) SPELL_G((p)->attr[ATTR_INT]) + HS(sp)

#define DMG_G(v) G(v)
#define DODGE_G(v) G(v)

#define DMG_BASE(p) DMG_G((p)->attr[ATTR_STR])
#define DODGE_BASE(p) DODGE_G((p)->attr[ATTR_DEX])

#define MODIFIER(ent, a) ((ent->attr[a] - 10) >> 1) // / 2

#define EQT(x)		(x>>6)
#define EQL(x)		(x & 15)
#define EQ(i, t)	(i | (t<<6))

extern long tmp_hds;

static inline unsigned
xsqrtx(unsigned x)
{
	return x * sqrt(x);
}

#define ENT_SETF(ref, field, value) { \
		dbref inner_ref = ref; \
		struct entity tmp = ent_get(inner_ref); \
		tmp.field = value; \
		ent_set(inner_ref, &tmp); \
	}
ENT ent_get(dbref ref);
void ent_set(dbref ref, ENT *tmp);
void ent_del(dbref ref);
void ent_tmp_reset(ENT *ent);

void birth(ENT *player);
int kill_dodge(dbref player_ref, char *wts);
short kill_dmg(enum element dmg_type,
		short dmg, short def,
		enum element def_type);

void recycle(dbref thing_ref);
void enter(dbref player_ref, dbref loc_ref, enum exit e);

const char * unparse(dbref loc_ref);

void sit(dbref player_ref, ENT *eplayer, char *what);
int stand_silent(dbref player_ref, ENT *eplayer);
void stand(dbref player_ref, ENT *eplayer);

int controls(dbref who_ref, dbref what_ref);
int payfor(dbref who_ref, OBJ *who, int cost);

void look_around(dbref player_ref);

int equip_affect(ENT *ewho, EQU *equ);
int equip(dbref player_ref, dbref eq_ref);
dbref unequip(dbref player_ref, unsigned eql);

enum element element_next(ENT *ref, register unsigned char a);

void stats_init(ENT *enu, SENT *sk);
void entity_update(dbref player_ref, double dt);
int entity_damage(dbref player_ref, ENT *eplayer, dbref target_ref, ENT *etarget, short amt);
/* void entity_gpt(OBJ *player, int echo_off, char *add_prompt); */

void entities_add(dbref where_ref, enum biome, long long pdn);
void enter_room(OBJ *player, enum exit e);
void entities_init();

#endif
