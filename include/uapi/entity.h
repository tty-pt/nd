#ifndef UAPI_ENTITY_H
#define UAPI_ENTITY_H

#include <math.h>
#include "uapi/skel.h"
#include "uapi/object.h"

#define EQUIP(ent, y) (ent)->equipment[y]
#define EFFECT(ent, w) (ent)->e[AF_ ## w]

#define STAT_ELEMENT(ent, type) \
	mask_element(ent, EFFECT(ent, type).mask)

#define G(x) xsqrtx(x)

#define HP_G(v) 10 * G(v)
#define HP_MAX(ent) ((unsigned short) HP_G((ent)->attr[ATTR_CON]))

#define MP_G(v) HP_G(v)
#define MP_MAX(ent) ((unsigned short) MP_G((ent)->attr[ATTR_WIZ]))

#define SPELL_G(v) G(v)
#define SPELL_DMG(p, sp) SPELL_G((p)->attr[ATTR_INT]) + HS(sp)

#define DMG_G(v) G(v)
#define DODGE_G(v) G(v)

#define RARE_MAX 6

#define MSRA(ms, ra, G) G(ms) * (ra + 1) / RARE_MAX

#define IE(equ, G) MSRA(equ->msv, equ->rare, G)
#define HS(sp) MSRA(sp->ms, sp->ra, SPELL_G)

#define DEF_G(v) G(v)
#define DEF_ARMOR(equ, aux) (IE(equ, DEF_G) >> aux)

#define DMG_BASE(p) DMG_G((p)->attr[ATTR_STR])
#define DODGE_BASE(p) DODGE_G((p)->attr[ATTR_DEX])

#define DMG_WEAPON(equ) IE(equ, DMG_G)
#define DODGE_ARMOR(def) def / 4

/* FIXME also refered to by eql */
enum bodypart {
	BP_NULL,
	BP_HEAD,
	BP_NECK,
	BP_CHEST,
	BP_BACK,
	BP_WEAPON,
	BP_LFINGER,
	BP_RFINGER,
	BP_LEGS,
};

ENT ent_get(unsigned ref);
void ent_set(unsigned ref, ENT *tmp);
void ent_del(unsigned ref);
void ent_reset(ENT *ent);
void birth(ENT *eplayer);

void sit(unsigned player_ref, ENT *eplayer, char *what);
int stand_silent(unsigned player_ref, ENT *eplayer);
void stand(unsigned player_ref, ENT *eplayer);

int controls(unsigned who_ref, unsigned what_ref);
int payfor(unsigned who_ref, OBJ *who, int cost);

void look_around(unsigned player_ref);

int equip_affect(ENT *ewho, EQU *equ);
int equip(unsigned player_ref, unsigned eq_ref);
unsigned unequip(unsigned player_ref, unsigned eql);

enum element mask_element(ENT *ref, register unsigned char a);

int entity_damage(unsigned player_ref, ENT *eplayer, unsigned target_ref, ENT *etarget, short amt);

void enter(unsigned player_ref, unsigned loc_ref, enum exit e);

int kill_dodge(unsigned player_ref, char *wts);
short kill_dmg(enum element dmg_type,
		short dmg, short def,
		enum element def_type);


static inline unsigned
xsqrtx(unsigned x)
{
	return x * sqrt(x);
}

/* spell */

int spell_cast(unsigned player_ref, ENT *eplayer, unsigned target_ref, unsigned slot);
void debufs_end(ENT *player);

#endif
