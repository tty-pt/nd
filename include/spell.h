#ifndef SPELL_H
#define SPELL_H

#include "geometry.h"
#include "stat.h"
#include "command.h"

#define ELEMENT(idx) (&element_map[idx])

#define ELEMENT_NEXT(ref, type) \
	element_next(ref, MOB_EM(MOB(ref), type))

enum element {
	ELM_PHYSICAL,
	ELM_FIRE,
	ELM_ICE,
	ELM_AIR,
	ELM_EARTH,
	ELM_SPIRIT,
	ELM_VAMP,
	ELM_DARK,
};

typedef struct {
	char *color;
	enum element weakness;
} element_t;

extern element_t element_map[];

enum element element_next(dbref ref, register unsigned char a);

#define DEBUF_DURATION(ra) 20 * (RARE_MAX - ra) / RARE_MAX
#define DEBUF_DMG(sp_dmg, duration) ((long) 2 * sp_dmg) / duration
#define DEBUF_TYPE_MASK 0xf
#define DEBUF_TYPE(sp) (sp->flags & DEBUF_TYPE_MASK)

#define SPELL(idx) (&spell_map[idx])
#define SPELL_G(v) G(v)
#define SPELL_DMG(p, sp) SPELL_G(GETSTAT(p, INT)) + HS(sp)
#define SPELL_COST(dmg, y, no_bdmg) (no_bdmg ? 0 : dmg) + dmg / (1 << y)

#define EFFECT(ref, idx) (&MOB(ref)->e[idx])

enum spell_affects {
	// these are changed by bufs
	AF_HP,
	AF_MOV,
	AF_MDMG,
	AF_MDEF,
	AF_DODGE,

	// these aren't.
	AF_DMG,
	AF_DEF,

	// these are flags, not types of buf
	AF_NEG = 0x10,
	AF_BUF = 0x20,
};

enum spell {
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

typedef struct {
	struct obj o;
	enum element element;
	unsigned char ms, ra, y, flags;
} spell_t;

struct debuf { // one for each type of spell
	spell_t *_sp;
	unsigned duration;
	short val;
};

typedef struct {
	spell_t *_sp;
	unsigned cost; 
	unsigned short val;
} spelli_t;

typedef struct {
	short value;
	unsigned char mask;
} effect_t;

void spells_init(spelli_t sps[8], dbref player);
void debuf_end(dbref who, unsigned i);
void debufs_end(dbref who);
void debufs_process(dbref who);
void debuf_notify(dbref who, struct debuf *d, short val);
int spell_cast(dbref attacker, dbref target, unsigned slot);
int spells_cast(dbref caster, dbref target);
int cspell_heal(dbref attacker, dbref target, short amt);

#endif
