#ifndef UAPI_ENTITY_H
#define UAPI_ENTITY_H

#include <math.h>
#include "./skel.h"
#include "./object.h"

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

typedef unsigned me_get_t(void);
me_get_t me_get;

typedef ENT ent_get_t(unsigned ref);
ent_get_t ent_get;

typedef void ent_set_t(unsigned ref, ENT *tmp);
ent_set_t ent_set;

typedef void ent_del_t(unsigned ref);
ent_del_t ent_del;

typedef void ent_reset_t(ENT *ent);
ent_reset_t ent_reset;

typedef void birth_t(ENT *eplayer);
birth_t birth;

typedef int controls_t(unsigned who_ref, unsigned what_ref);
controls_t controls;

typedef int payfor_t(unsigned who_ref, OBJ *who, unsigned cost);
payfor_t payfor;

typedef void look_around_t(unsigned player_ref);
look_around_t look_around;

typedef unsigned mask_element_t(ENT *ref, register unsigned char a);
mask_element_t mask_element;

typedef int entity_damage_t(unsigned player_ref, ENT *eplayer, unsigned target_ref, ENT *etarget, short amt);
entity_damage_t entity_damage;

typedef void enter_t(unsigned player_ref, unsigned loc_ref, enum exit e);
enter_t enter;

typedef int kill_dodge_t(unsigned player_ref, char *wts);
kill_dodge_t kill_dodge;

typedef short kill_dmg_t(unsigned dmg_type,
		short dmg, short def,
		unsigned def_type);
kill_dmg_t kill_dmg;

typedef void look_at_t(unsigned player_ref, unsigned loc_ref);
look_at_t look_at;

static inline unsigned
xsqrtx(unsigned x)
{
	return x * sqrt(x);
}

/* spell */

typedef int spell_cast_t(unsigned player_ref, ENT *eplayer, unsigned target_ref, unsigned slot);
spell_cast_t spell_cast;

typedef void debufs_end_t(ENT *player);
debufs_end_t debufs_end;

extern unsigned me;
extern unsigned ent_hd;

#endif
