#include <ndc.h>

#include <string.h>
#include <stdlib.h>

#include "io.h"
#include "spell.h"
#include "entity.h"
#include "mob.h"
#include "params.h"
#include "debug.h"
#include "uapi/wts.h"
/* #include "speech.h" */

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

unsigned heal_id;

SKEL spell_map[] = {
	[SPELL_HEAL] = {
		.name = "Heal",
		.description = "",
		.type = STYPE_SPELL,
		.sp = {
			.spell = {
				.element = ELM_PHYSICAL,
				.ms = 3, .ra = 1, .y = 2,
				.flags = AF_HP,
			}
		},
	},

	[SPELL_FOCUS] = {
		.name = "Focus",
		.description = "",
		.type = STYPE_SPELL,
		.sp = {
			.spell = {
				.element = ELM_PHYSICAL,
				.ms = 15, .ra = 3, .y = 1,
				.flags = AF_MDMG | AF_BUF,
			}
		},
	},

	[SPELL_FIRE_FOCUS] = {
		.name = "Fire Focus",
		.description = "",
		.type = STYPE_SPELL,
		.sp = {
			.spell = {
				.element = ELM_FIRE,
				.ms = 15, .ra = 3, .y = 1,
				.flags = AF_MDMG | AF_BUF,
			}
		},
	},

	[SPELL_CUT] = {
		.name = "Cut",
		.description = "",
		.type = STYPE_SPELL,
		.sp = {
			.spell = {
				.element = ELM_PHYSICAL,
				.ms = 15, .ra = 1, .y = 2,
				.flags = AF_NEG,
			}
		},
	},

	[SPELL_FIREBALL] = {
		.name = "Fireball",
		.description = "",
		.type = STYPE_SPELL,
		.sp = {
			.spell = {
				.element = ELM_FIRE,
				.ms = 3, .ra = 1, .y = 2,
				.flags = AF_NEG,
			}
		},
	}, // 1/4 chance of burning

	[SPELL_WEAKEN] = {
		.name = "Weaken",
		.description = "",
		.type = STYPE_SPELL,
		.sp = { .spell = {
			.element = ELM_PHYSICAL,
			.ms = 15, .ra = 3, .y = 1,
			.flags = AF_MDMG | AF_BUF | AF_NEG,
		} },
	},

	[SPELL_DISTRACT] = {
		.name = "Distract",
		.description = "",
		.type = STYPE_SPELL,
		.sp = { .spell = {
			.element = ELM_PHYSICAL,
			.ms = 15, .ra = 3, .y = 1,
			.flags = AF_MDEF | AF_BUF | AF_NEG,
		} },
	},

	[SPELL_FREEZE] = {
		.name = "Freeze",
		.description = "",
		.type = STYPE_SPELL,
		.sp = { .spell = {
			.element = ELM_ICE,
			.ms = 10, .ra = 2, .y = 4,
			.flags = AF_MOV | AF_NEG,
		} },
	},

	[SPELL_LAVA_SHIELD] = {
		.name = "Lava Shield",
		.description = "",
		.type = STYPE_SPELL,
		.sp = { .spell = {
			.element = ELM_FIRE,
			.ms = 15, .ra = 3, .y = 1,
			.flags = AF_MDEF | AF_BUF,
		} },
	},

	[SPELL_WIND_VEIL] = {
		.name = "Wind Veil",
		.description = "",
		.type = STYPE_SPELL,
		.sp = { .spell = {
			.flags = AF_DODGE,
		} },
	},

	[SPELL_STONE_SKIN] = {
		.name = "Stone Skil",
		.description = "",
		.type = STYPE_SPELL,
		.sp = { .spell = {
			.flags = AF_DEF,
		} },
	},
};

char *buf_wts[] = {
	// HP
	"heal",

	// - HP
	[16] =
	"bleed",
	"burn",

	// 32 MOV

	// - MOV
	[48] =
	"stunned",
	"",
	"frozen",

	// MDMG
	[64] =
	"focus on attacking",
	"focus on fire",

	// - MDMG
	[80] =
	"weaken",

	// MDEF
	[96] =
	"focus on defending",
	"are protected by flames",

	// - MDEF
	[112] =
	"are distracted",

	// 128 DODGE

	// 144 - DDGE
};

enum element
element_next(ENT *ent, register unsigned char a)
{
	unsigned skel_id = ent->debufs[__builtin_ffs(a) - 1].skel;
	SKEL skel = skel_get(skel_id);
	SSPE *sp = &skel.sp.spell;
	return a ? sp->element : ELM_PHYSICAL;
}

void
debuf_end(ENT *eplayer, unsigned i)
{
	struct debuf *d = &eplayer->debufs[i];
	SKEL skel = skel_get(d->skel);
	SSPE *sp = &skel.sp.spell;
	struct effect *e = &eplayer->e[DEBUF_TYPE(sp)];
	i = 1 << i;

	eplayer->debuf_mask ^= i;
	e->mask ^= i;
	e->value -= d->val;
}

static inline char const *
sp_color(struct spell_skeleton *_sp)
{
	return DEBUF_TYPE(_sp) == AF_HP
		&& !(_sp->flags & AF_NEG)
		? ANSI_BOLD ANSI_FG_GREEN
		: ELEMENT(_sp->element)->color;
}

static inline char*
debuf_wts(struct spell_skeleton *_sp)
{
	register unsigned char mask = _sp->flags;
	register unsigned idx = (DEBUF_TYPE(_sp) << 1) + ((mask >> 4) & 1);
	idx = (idx << 4) + _sp->element;
	return buf_wts[idx];
}

void
debuf_notify(dbref player_ref, struct debuf *d, short val)
{
	char buf[BUFSIZ];
	SKEL skel = skel_get(d->skel);
	SSPE *_sp = &skel.sp.spell;
	char const *color = sp_color(_sp);
	char *wts = debuf_wts(_sp);

	if (val)
		snprintf(buf, sizeof(buf), " (%s%d%s)", color, val, ANSI_RESET);
	else
		*buf = '\0';

	notify_wts(player_ref, wts, wts_plural(wts), "%s", buf);
}

int
debufs_process(dbref player_ref, ENT *eplayer)
{
	register unsigned mask, i, aux;
	short hpi = 0, dmg;
	struct debuf *d, *hd;

	for (mask = eplayer->debuf_mask, i = 0;
	     (aux = __builtin_ffs(mask));
	     i++, mask >>= aux)
	{
		i += aux - 1;
		d = &eplayer->debufs[i];
		d->duration--;
		if (d->duration <= 0) {
			debuf_end(eplayer, i);
			continue;
		}
		SKEL skel = skel_get(d->skel);
		SSPE *sp = &skel.sp.spell;
		// wtf is this special code?
		if (DEBUF_TYPE(sp) == AF_HP) {
			dmg = kill_dmg(sp->element, d->val,
				      EFFECT(eplayer, MDEF).value,
				      ELEMENT_NEXT(eplayer, MDEF));
			hd = d;

			hpi += dmg;
		}
	}

	if (hpi) {
		debuf_notify(player_ref, hd, hpi);
		return entity_damage(NOTHING, NULL, player_ref, eplayer, hpi);
	}

	return 0;
}

void
debufs_end(ENT *eplayer)
{
	register unsigned mask, i, aux;

	for (mask = eplayer->debuf_mask, i = 0;
	     (aux = __builtin_ffs(mask));
	     i++, mask >>= aux)

		 debuf_end(eplayer, i += aux - 1);
}

static inline int
debuf_start(dbref player_ref, struct spell *sp, short val)
{
	SKEL skel = skel_get(sp->skel);
	SSPE *_sp = &skel.sp.spell;
	ENT eplayer = ent_get(player_ref);
	struct debuf *d;
	int i;

	if (eplayer.debuf_mask) {
		i = __builtin_ffs(~eplayer.debuf_mask);
		if (!i)
			return -1;
		i--;
	} else
		i = 0;

	d = &eplayer.debufs[i];
	d->skel = sp->skel;
	d->duration = DEBUF_DURATION(_sp->ra);
	d->val = DEBUF_DMG(val, d->duration);

	i = 1 << i;
	eplayer.debuf_mask |= i;

	struct effect *e = &eplayer.e[DEBUF_TYPE(_sp)];
	e->mask |= i;
	e->value += d->val;

	debuf_notify(player_ref, d, 0);
	ent_set(player_ref, &eplayer);

	return 0;
}

int
spell_cast(dbref player_ref, ENT *eplayer, dbref target_ref, unsigned slot)
{
	ENT etarget = ent_get(target_ref);
	struct spell sp = eplayer->spells[slot];
	SKEL skel = skel_get(sp.skel);
	SSPE *_sp = &skel.sp.spell;

	unsigned mana = eplayer->mp;
	char a[BUFSIZ]; // FIXME way too big?
	char c[BUFSIZ + 32];

	char const *color = sp_color(_sp);

	if (mana < sp.cost)
		return -1;

	snprintf(a, sizeof(a), "%s%s"ANSI_RESET, color, skel.name);

	mana -= sp.cost;
	eplayer->mp = mana > 0 ? mana : 0;

	short val = kill_dmg(_sp->element, sp.val,
			EFFECT(&etarget, MDEF).value,
			ELEMENT_NEXT(&etarget, MDEF));

	if (_sp->flags & AF_NEG) {
		val = -val;
		if (kill_dodge(player_ref, a))
			return 0;
	} else
		target_ref = player_ref;

	snprintf(c, sizeof(c), "cast %s on", a);

	notify_attack(player_ref, target_ref, c, 0, color, val);

	if (random() < (RAND_MAX >> _sp->y))
		debuf_start(target_ref, &sp, val);

	return entity_damage(player_ref, eplayer, target_ref, &etarget, val);
}

int
spells_cast(dbref player_ref, ENT *eplayer, dbref target_ref)
{
	register unsigned i, d, combo = eplayer->combo;
	unsigned enough_mp = 1;

	for (i = 0; enough_mp && (d = __builtin_ffs(combo)); combo >>= d) {
		switch (spell_cast(player_ref, eplayer, target_ref, i) - 1) {
		case -1: enough_mp = 0;
			 break;
		case 1:  return 0;
		}
	}

	if (!enough_mp)
		nd_writef(player_ref, "Not enough mana.\n");

	return 1;
}

void
spells_birth(ENT *entity) {
	register int j;
	for (j = 0; j < 8; j++) {
		struct spell *sp = &entity->spells[j];
		SKEL skel = skel_get(heal_id);
		struct spell_skeleton *_sp = &skel.sp.spell;
		sp->skel = 0;
		sp->val = SPELL_DMG(entity, _sp);
		sp->cost = SPELL_COST(sp->val, _sp->y, _sp->flags & AF_BUF);
	}
}

void
spells_init() {
	for (int i = 0; i < SPELL_MAX; i++)
		skel_new(&spell_map[i]);
}
