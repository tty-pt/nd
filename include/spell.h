#ifndef SPELL_H
#define SPELL_H

#include "geometry.h"
#include "stat.h"

#define ELEMENT(idx) (&element_map[idx])

#define ELEMENT_NEXT(ref, type) \
	element_next(ref, MOBI_EM(MOBI(ref), type))

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

enum element element_next(ref_t ref, register unsigned char a);

#define DEBUF_DURATION(ra) 20 * (RARE_MAX - ra) / RARE_MAX
#define DEBUF_DMG(sp_dmg, duration) ((long) 2 * sp_dmg) / duration
#define DEBUF_TYPE_MASK 0xf
#define DEBUF_TYPE(sp) (sp->flags & DEBUF_TYPE_MASK)

#define SPELL(idx) (&spell_map[idx])
#define SPELL_G(v) G(v)
#define SPELL_DMG(p, sp) SPELL_G(GETSTAT(p, INT)) + HS(sp)
#define SPELL_COST(dmg, y, no_bdmg) (no_bdmg ? 0 : dmg) + dmg / (1 << y)

#define EFFECT(ref, idx) (&MOBI(ref)->e[idx])

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

void spells_init(spelli_t sps[8], ref_t player);
void debuf_end(ref_t who, unsigned i);
void debufs_end(ref_t who);
void debufs_process(ref_t who);
void debuf_notify(dbref who, struct debuf *d, short val);
int spell_cast(ref_t attacker, ref_t target, unsigned slot);
int spells_cast(ref_t caster, ref_t target);
int cspell_heal(ref_t attacker, ref_t target, short amt);

#endif
