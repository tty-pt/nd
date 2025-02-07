#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "params.h"
#include "uapi/entity.h"
#include "uapi/io.h"
#include "uapi/wts.h"

#define DEBUF_DURATION(ra) 20 * (RARE_MAX - ra) / RARE_MAX
#define DEBUF_DMG(sp_dmg, duration) ((long) 2 * sp_dmg) / duration
#define DEBUF_TYPE_MASK 0xf
#define DEBUF_TYPE(sp) (sp->flags & DEBUF_TYPE_MASK)

#define SPELL_COST(dmg, y, no_bdmg) (no_bdmg ? 0 : dmg) + dmg / (1 << y)

#define HEAL_SKEL_REF 1

unsigned
mask_element(ENT *ent, register unsigned char a)
{
	unsigned skel_id = ent->debufs[__builtin_ffs(a) - 1].skel;
	SKEL skel;
	qdb_get(skel_hd, &skel, &skel_id);
	SSPE *sp = &skel.sp.spell;
	return a ? sp->element : ELM_PHYSICAL;
}

void
debuf_end(ENT *eplayer, unsigned i)
{
	struct debuf *d = &eplayer->debufs[i];
	SKEL skel;
	qdb_get(skel_hd, &skel, &d->skel);
	SSPE *sp = &skel.sp.spell;
	struct effect *e = &eplayer->e[DEBUF_TYPE(sp)];
	i = 1 << i;

	eplayer->debuf_mask ^= i;
	e->mask ^= i;
	e->value -= d->val;
}

static inline enum color
sp_color(struct spell_skeleton *_sp)
{
	if (DEBUF_TYPE(_sp) != AF_HP || (_sp->flags & AF_NEG)) {
		element_t element;
		qdb_get(element_hd, &element, &_sp->element);
		return element.color;
	}

	return GREEN;
}

static inline char*
debuf_wts(struct spell_skeleton *_sp)
{
	static char ret[BUFSIZ];
	register unsigned char mask = _sp->flags;
	register unsigned idx = (DEBUF_TYPE(_sp) << 1) + ((mask >> 4) & 1);
	unsigned wts_ref;
	extern unsigned awts_hd, wts_hd;
	unsigned ref = (_sp->element << 4) | idx;
	qdb_get(awts_hd, &wts_ref, &ref);
	qdb_get(wts_hd, ret, &wts_ref);
	return ret;
}

void
debuf_notify(unsigned player_ref, struct debuf *d, short val)
{
	char buf[BUFSIZ];
	SKEL skel;
	qdb_get(skel_hd, &skel, &d->skel);
	SSPE *_sp = &skel.sp.spell;
	char *wts = debuf_wts(_sp);

	if (val)
		snprintf(buf, sizeof(buf), " (%s%d%s)", ansi_fg[sp_color(_sp)], val, ANSI_RESET);
	else
		*buf = '\0';

	notify_wts(player_ref, wts, wts_plural(wts), "%s", buf);
}

int
debufs_process(unsigned player_ref, ENT *eplayer)
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
		SKEL skel;
		qdb_get(skel_hd, &skel, &d->skel);
		SSPE *sp = &skel.sp.spell;
		// wtf is this special code?
		if (DEBUF_TYPE(sp) == AF_HP) {
			dmg = kill_dmg(sp->element, d->val,
				      EFFECT(eplayer, MDEF).value,
				      STAT_ELEMENT(eplayer, MDEF));
			hd = d;

			hpi += dmg;
		}
	}

	if (hpi) {
		debuf_notify(player_ref, hd, hpi);
		return hpi;
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
debuf_start(unsigned player_ref, struct spell *sp, short val)
{
	SKEL skel;
	qdb_get(skel_hd, &skel, &sp->skel);
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
spell_cast(unsigned player_ref, ENT *eplayer, unsigned target_ref, unsigned slot)
{
	ENT etarget = ent_get(target_ref);
	struct spell sp = eplayer->spells[slot];
	SKEL skel;
	qdb_get(skel_hd, &skel, &sp.skel);
	SSPE *_sp = &skel.sp.spell;

	unsigned mana = eplayer->mp;
	char a[BUFSIZ]; // FIXME way too big?
	char c[BUFSIZ + 32];

	enum color color = sp_color(_sp);

	if (mana < sp.cost)
		return -1;

	snprintf(a, sizeof(a), "%s%s"ANSI_RESET, ansi_fg[color], skel.name);

	mana -= sp.cost;
	eplayer->mp = mana > 0 ? mana : 0;

	short val = kill_dmg(_sp->element, sp.val,
			EFFECT(&etarget, MDEF).value,
			STAT_ELEMENT(&etarget, MDEF));

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
spells_cast(unsigned player_ref, ENT *eplayer, unsigned target_ref)
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
		SKEL skel;
		unsigned ref = HEAL_SKEL_REF;
		qdb_get(skel_hd, &skel, &ref);
		struct spell_skeleton *_sp = &skel.sp.spell;
		sp->val = SPELL_DMG(entity, _sp);
		sp->cost = SPELL_COST(sp->val, _sp->y, _sp->flags & AF_BUF);
	}
}
