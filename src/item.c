#include "item.h"

#include <stddef.h> // offsetof
#include "props.h"
#include "externs.h"
#include "kill.h"
#include "mdb.h"
#include "interface.h"
#include "geography.h"
#include "item.h"
#include "mob.h"
#include "web.h"

#define DMG_WEAPON(x) IE(x, DMG_G)
#define WTS_WEAPON(eq) phys_wts[EQT(EQUIPMENT(eq)->eqw)]

#define DEF_G(v) G(v)
#define DEF_ARMOR(x, aux) (IE(x, DEF_G) >> aux)
#define DODGE_ARMOR(def) def / 4

int
equip_affect(dbref who, dbref eq)
{
	struct entity *p = ENTITY(who);
	register int msv = EQUIPMENT(eq)->msv,
		 eqw = EQUIPMENT(eq)->eqw,
		 eql = EQL(eqw),
		 eqt = EQT(eqw);
	unsigned aux = 0;

	switch (eql) {
	case ES_RHAND:
		if (ATTR(who, ATTR_STR) < msv)
			return 1;
		EFFECT(p, DMG).value += DMG_WEAPON(eq);
		p->wts = WTS_WEAPON(eq);
		break;

	case ES_HEAD:
	case ES_PANTS:
		aux = 1;
	case ES_CHEST:

		switch (eqt) {
		case ARMOR_LIGHT:
			if (ATTR(who, ATTR_DEX) < msv)
				return 1;
			aux += 2;
			break;
		case ARMOR_MEDIUM:
			msv /= 2;
			if (ATTR(who, ATTR_STR) < msv
			    || ATTR(who, ATTR_DEX) < msv)
				return 1;
			aux += 1;
			break;
		case ARMOR_HEAVY:
			if (ATTR(who, ATTR_STR) < msv)
				return 1;
		}
		aux = DEF_ARMOR(eq, aux);
		EFFECT(p, DEF).value += aux;
		int pd = EFFECT(p, DODGE).value - DODGE_ARMOR(aux);
		EFFECT(p, DODGE).value = pd > 0 ? pd : 0;
	}

	return 0;
}

int
equip(dbref who, dbref eq)
{
	CBUG(OBJECT(who)->type != TYPE_ENTITY);
	unsigned eql = EQL(EQUIPMENT(eq)->eqw);

	if (!eql || EQUIP(who, eql) > 0
	    || equip_affect(who, eq))
		return 1;

	EQUIP(who, eql) = eq;
	EQUIPMENT(eq)->flags |= EF_EQUIPPED;

	notifyf(who, "You equip %s.", OBJECT(eq)->name);
	web_stats(who);
	web_content_out(who, eq);
	web_equipment(who);
	return 0;
}

void
do_select(command_t *cmd)
{
	dbref player = cmd->player;
	const char *n_s = cmd->argv[1];
	unsigned n = strtoul(n_s, NULL, 0);
	struct entity *mob = ENTITY(player);
	mob->select = n;
	notifyf(player, "You select %u.", n);
}

void
do_equip(command_t *cmd)
{
	dbref player = cmd->player;
	char const *name = cmd->argv[1];
	dbref eq = ematch_at(player, player, name);

	if (eq < 0) {
		notify(player, "You are not carrying that.");
		return;
	}

	if (equip(player, eq)) { 
		notify(player, "You can't equip that.");
		return;
	}
}

dbref
unequip(dbref who, unsigned eql)
{
	CBUG(OBJECT(who)->type != TYPE_ENTITY);
	struct entity *mob = ENTITY(who);
	dbref eq = EQUIP(who, eql);
	unsigned eqt, aux;

	if (!eq)
		return NOTHING;

	eqt = EQT(EQUIPMENT(eq)->eqw);
	aux = 0;
	assert(mob);

	switch (eql) {
	case ES_RHAND:
		EFFECT(mob, DMG).value -= DMG_WEAPON(eq);
		mob->wts = phys_wts[mob->wtso];
		break;
	case ES_PANTS:
	case ES_HEAD:
		aux = 1;
	case ES_CHEST:
		switch (eqt) {
		case ARMOR_LIGHT: aux += 2; break;
		case ARMOR_MEDIUM: aux += 1; break;
		}
		aux = DEF_ARMOR(eq, aux);
		EFFECT(mob, DEF).value -= aux;
		EFFECT(mob, DODGE).value += DODGE_ARMOR(aux);
	}

	EQUIP(who, eql) = NOTHING;
	EQUIPMENT(eq)->flags &= ~EF_EQUIPPED;
	web_content_in(who, eq);
	web_stats(who);
	web_equipment(who);
	return eq;
}

void
do_unequip(command_t *cmd)
{
	dbref player = cmd->player;
	char const *name = cmd->argv[1];
	enum bodypart bp = BODYPART_ID(*name);
	dbref eq;

	if ((eq = unequip(player, bp)) == NOTHING) {
		notify(player, "You don't have that equipped.");
		return;
	}

	notifyf(player, "You unequip %s.", OBJECT(eq)->name);
}
