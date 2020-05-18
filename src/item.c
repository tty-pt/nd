#include "item.h"

#include <stddef.h> // offsetof
#include "props.h"
#include "externs.h"
#include "kill.h"
#include "db.h"
#include "interface.h"
#include "geography.h"
#include "item.h"
#include "mob.h"
#undef NDEBUG
#include "debug.h"

#define DMG_WEAPON(x) IE(x, DMG_G)
#define WTS_WEAPON(eq) phys_wts[GETEQT(eq)]

#define DEF_G(v) G(v)
#define DEF_ARMOR(x, aux) (IE(x, DEF_G) >> aux)
#define DODGE_ARMOR(def) def / 4

int
equip_calc(dbref who, dbref eq)
{
	mobi_t *p = MOBI(who);
	register int msv = GETMSV(eq),
		 eqw = GETEQW(eq),
		 eql = EQL(eqw),
		 eqt = EQT(eqw);
	unsigned aux = 0;

	switch (eql) {
	case RHAND:
		if (GETSTAT(p->who, STR) < msv)
			return 1;
		MOBI_EV(p, DMG) += DMG_WEAPON(eq);
		p->wts = WTS_WEAPON(eq);
		break;

	case HEAD:
	case PANTS:
		aux = 1;
	case CHEST:

		switch (eqt) {
		case ARMOR_LIGHT:
			if (GETSTAT(p->who, DEX) < msv)
				return 1;
			aux += 2;
			break;
		case ARMOR_MEDIUM:
			msv /= 2;
			if (GETSTAT(p->who, STR) < msv
			    || GETSTAT(p->who, DEX) < msv)
				return 1;
			aux += 1;
			break;
		case ARMOR_HEAVY:
			if (GETSTAT(p->who, STR) < msv)
				return 1;
		}
		aux = DEF_ARMOR(eq, aux);
		MOBI_EV(p, DEF) += aux;
		int pd = MOBI_EV(p, DODGE) - DODGE_ARMOR(aux);
		MOBI_EV(p, DODGE) = pd > 0 ? pd : 0;
	}

	return 0;
}

int
cannot_equip(dbref who, dbref eq)
{
	unsigned eql = GETEQL(eq);

	if (GETEQ(who, eql)
	    || equip_calc(who, eq))
		return 1;

	SETEQ(who, eql, eq);
	DBFETCH(eq)->flags |= DARK;
	DBDIRTY(eq);

	notify_fmt(who, "You equip %s.", NAME(eq));
	return 0;
}

void
do_select(dbref player, const char *n_s)
{
	unsigned n = strtoul(n_s, NULL, 0);
	mobi_t *liv = MOBI(player);
	liv->select = n;
	notify_fmt(player, "You select %u.", n);
}

void
do_equip(int descr, dbref player, char const *name)
{
	dbref eq = contents_find(descr, player, player, name);

	if (eq < 0) {
		notify(player, "You are not carrying that.");
		return;
	}

	if (cannot_equip(player, eq)) { 
		notify(player, "You can't equip that.");
		return;
	}
}

dbref
unequip(dbref who, unsigned eql)
{
	mobi_t *liv = MOBI(who);
	dbref eq = GETEQ(who, eql);
	unsigned eqt, aux;

	if (!eq)
		return NOTHING;

	eqt = EQT(GETEQW(eq));
	aux = 0;
	assert(liv);

	switch (eql) {
	case RHAND:
		MOBI_EV(liv, DMG) -= DMG_WEAPON(eq);
		liv->wts = phys_wts[liv->mob ? liv->mob->wt : GETWTS(who)];
		break;
	case PANTS:
	case HEAD:
		aux = 1;
	case CHEST:
		switch (eqt) {
		case ARMOR_LIGHT: aux += 2; break;
		case ARMOR_MEDIUM: aux += 1; break;
		}
		aux = DEF_ARMOR(eq, aux);
		MOBI_EV(liv, DEF) -= aux;
		MOBI_EV(liv, DODGE) += DODGE_ARMOR(aux);
	}

	SETEQ(who, eql, 0);
	DBFETCH(eq)->flags &= ~DARK;
	DBDIRTY(eq);
	return eq;
}

void
do_unequip(int descr, dbref player, char const *name)
{
	enum bodypart bp = BODYPART_ID(*name);
	dbref eq;

	if ((eq = unequip(player, bp)) == NOTHING) {
		notify(player, "You don't have that equipped.");
		return;
	}

	notify_fmt(player, "You unequip %s.", NAME(eq));
}
