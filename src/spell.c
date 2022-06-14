#include "io.h"

#include "spell.h"
#include "entity.h"

#include "mob.h"

#include "params.h"
/* #include "speech.h" */
#include "externs.h"

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

struct spell_skeleton spell_skeleton_map[] = {
	[SPELL_HEAL] = {
		{"Heal", "", "" },
		ELM_PHYSICAL, 3, 1, 2, AF_HP,
	},

	[SPELL_FOCUS] = {
		{"Focus", "", "" },
		ELM_PHYSICAL, 15, 3, 1,
		AF_MDMG | AF_BUF,
	},

	[SPELL_FIRE_FOCUS] = {
		{"Fire Focus", "", "" },
		ELM_FIRE, 15, 3, 1,
		AF_MDMG | AF_BUF,
	},

	[SPELL_CUT] = {
		{"Cut", "", "" },
		ELM_PHYSICAL, 15, 1, 2,
		AF_NEG,
	},

	[SPELL_FIREBALL] = {
		{"Fireball", "", "" },
		ELM_FIRE, 3, 1, 2,
		AF_NEG,
	}, // 1/4 chance of burning

	[SPELL_WEAKEN] = {
		{"Weaken", "", "" },
		ELM_PHYSICAL, 15, 3, 1,
		AF_MDMG | AF_BUF | AF_NEG,
	},

	[SPELL_DISTRACT] = {
		{"Distract", "", "" },
		ELM_PHYSICAL, 15, 3, 1,
		AF_MDEF | AF_BUF | AF_NEG,
	},

	[SPELL_FREEZE] = {
		{"Freeze", "", "" },
		ELM_ICE, 10, 2, 4,
		AF_MOV | AF_NEG,
	},

	[SPELL_LAVA_SHIELD] = {
		{"Lava Shield", "", "" },
		ELM_FIRE, 15, 3, 1,
		AF_MDEF | AF_BUF,
	},

	[SPELL_WIND_VEIL] = {
		{"Wind Veil", "", "" },
		.flags = AF_DODGE,
	},

	[SPELL_STONE_SKIN] = {
		{"Stone Skin", "", "" },
		.flags = AF_DEF,
	},
};

struct wts buf_wts[] = {
	// HP
	{ "heal", "heals" },

	// - HP
	[16] =
	{ "bleed", "bleeds" },
	{ "burn", "burns" },

	// 32 MOV

	// - MOV
	[48] =
	{ "stunned", "stunned" },
	{ "", "" },
	{ "frozen", "frozen" },

	// MDMG
	[64] =
	{ "focus on attacking", "focuses on attacking" },
	{ "focus on fire", "focuses on fire" },

	// - MDMG
	[80] =
	{ "weaken", "weakens" },

	// MDEF
	[96] =
	{ "focus on defending", "focuses on defending" },
	{ "are protected by flames", "is protected by flames" },

	// - MDEF
	[112] =
	{ "are distracted", "is distracted" },

	// 128 DODGE

	// 144 - DDGE
};

enum element
element_next(ENT *ent, register unsigned char a)
{
	return a ? ent->debufs[__builtin_ffs(a) - 1]._sp->element : ELM_PHYSICAL;
}

void
debuf_end(ENT *eplayer, unsigned i)
{
	struct debuf *d = &eplayer->debufs[i];
	struct effect *e = &eplayer->e[DEBUF_TYPE(d->_sp)];
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

static inline struct wts *
debuf_wts(struct spell_skeleton *_sp)
{
	register unsigned char mask = _sp->flags;
	register unsigned idx = (DEBUF_TYPE(_sp) << 1) + ((mask >> 4) & 1);
	idx = (idx << 4) + _sp->element;
	return &buf_wts[idx];
}

void
debuf_notify(OBJ *player, struct debuf *d, short val)
{
	char buf[BUFSIZ];
	register struct spell_skeleton *_sp = d->_sp;
	char const *color = sp_color(_sp);
	struct wts *wts = debuf_wts(_sp);

	if (val)
		snprintf(buf, sizeof(buf), " (%s%d%s)", color, val, ANSI_RESET);
	else
		*buf = '\0';

	notify_wts(player, wts->a, wts->b, "%s", buf);
}

int
debufs_process(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
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
		if (d->duration == 0)
			debuf_end(eplayer, i);
		// wtf is this special code?
		else if (DEBUF_TYPE(d->_sp) == AF_HP) {
			dmg = kill_dmg(d->_sp->element, d->val,
				      EFFECT(eplayer, MDEF).value,
				      ELEMENT_NEXT(eplayer, MDEF));
			hd = d;

			hpi += dmg;
		}
	}

	if (hpi) {
		debuf_notify(player, hd, hpi);
		return entity_damage(NULL, player, hpi);
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
debuf_start(OBJ *player, struct spell *sp, short val)
{
	struct spell_skeleton *_sp = sp->_sp;
	ENT *eplayer = &player->sp.entity;
	struct debuf *d;
	int i;

	if (eplayer->debuf_mask) {
		i = __builtin_ffs(~eplayer->debuf_mask);
		if (!i)
			return -1;
		i--;
	} else
		i = 0;

	d = &eplayer->debufs[i];
	d->_sp = _sp;
	d->duration = DEBUF_DURATION(_sp->ra);
	d->val = DEBUF_DMG(val, d->duration);

	i = 1 << i;
	eplayer->debuf_mask |= i;

	struct effect *e = &eplayer->e[DEBUF_TYPE(_sp)];
	e->mask |= i;
	e->value += d->val;

	debuf_notify(player, d, 0);

	return 0;
}

int
spell_cast(OBJ *player, OBJ *target, unsigned slot)
{
	ENT *eplayer = &player->sp.entity,
	    *etarget = &target->sp.entity;
	struct spell sp = eplayer->spells[slot];
	struct spell_skeleton *_sp = sp._sp;

	unsigned mana = eplayer->mp;
	char a[BUFSIZ]; // FIXME way too big?
	char b[BUFSIZ];
	char c[BUFSIZ];
	struct wts wts = { a, b };

	char const *color = sp_color(_sp);

	if (mana < sp.cost)
		return -1;

	snprintf(a, sizeof(a), "%s%s"ANSI_RESET, color, _sp->o.name);
	memcpy(b, a, sizeof(a));

	mana -= sp.cost;
	eplayer->mp = mana > 0 ? mana : 0;

	short val = kill_dmg(_sp->element, sp.val,
			EFFECT(etarget, MDEF).value,
			ELEMENT_NEXT(etarget, MDEF));

	if (_sp->flags & AF_NEG) {
		val = -val;
		if (kill_dodge(player, wts))
			return 0;
	} else
		target = player;

	snprintf(b, sizeof(b), "casts %s on", a);
	snprintf(c, sizeof(c), "cast %s on", a);
	wts.a = c;

	notify_attack(player, target, wts, 0, color, val);

	if (random() < (RAND_MAX >> _sp->y))
		debuf_start(target, &sp, val);

	return entity_damage(player, target, val);
}

int
spells_cast(OBJ *player, OBJ *target)
{
	ENT *eplayer = &player->sp.entity;
	register unsigned i, d, combo = eplayer->combo;
	unsigned enough_mp = 1;

	for (i = 0; enough_mp && (d = __builtin_ffs(combo)); combo >>= d) {
		switch (spell_cast(player, target, (i += d) - 1)) {
		case -1: enough_mp = 0;
			 break;
		case 1:  return 1;
		}
	}

	if (!enough_mp)
		notify(eplayer, "Not enough mana.");

	return 0;
}

