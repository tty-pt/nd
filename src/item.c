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

#define DMG_WEAPON(equ) IE(equ, DMG_G)
#define WTS_WEAPON(equ) phys_wts[EQT(equ->eqw)]

#define DEF_G(v) G(v)
#define DEF_ARMOR(equ, aux) (IE(equ, DEF_G) >> aux)
#define DODGE_ARMOR(def) def / 4

int
equip_affect(OBJ *who, OBJ *eq)
{
	ENT *ewho = &who->sp.entity;
	EQU *eeq = &eq->sp.equipment;

	register int msv = eeq->msv,
		 eqw = eeq->eqw,
		 eql = EQL(eqw),
		 eqt = EQT(eqw);

	unsigned aux = 0;

	switch (eql) {
	case ES_RHAND:
		if (ewho->attr[ATTR_STR] < msv)
			return 1;
		EFFECT(ewho, DMG).value += DMG_WEAPON(eeq);
		ewho->wts = WTS_WEAPON(eeq);
		break;

	case ES_HEAD:
	case ES_PANTS:
		aux = 1;
	case ES_CHEST:

		switch (eqt) {
		case ARMOR_LIGHT:
			if (ewho->attr[ATTR_DEX] < msv)
				return 1;
			aux += 2;
			break;
		case ARMOR_MEDIUM:
			msv /= 2;
			if (ewho->attr[ATTR_STR] < msv
			    || ewho->attr[ATTR_DEX] < msv)
				return 1;
			aux += 1;
			break;
		case ARMOR_HEAVY:
			if (ewho->attr[ATTR_STR] < msv)
				return 1;
		}
		aux = DEF_ARMOR(eeq, aux);
		EFFECT(ewho, DEF).value += aux;
		int pd = EFFECT(ewho, DODGE).value - DODGE_ARMOR(aux);
		EFFECT(ewho, DODGE).value = pd > 0 ? pd : 0;
	}

	return 0;
}

int
equip(OBJ *who, OBJ *eq)
{
	CBUG(who->type != TYPE_ENTITY);
	ENT *ewho = &who->sp.entity;
	CBUG(eq->type != TYPE_EQUIPMENT);
	EQU *eeq = &eq->sp.equipment;
	unsigned eql = EQL(eeq->eqw);

	if (!eql || EQUIP(ewho, eql) > 0
	    || equip_affect(who, eq))
		return 1;

	EQUIP(ewho, eql) = object_ref(eq);
	eeq->flags |= EF_EQUIPPED;

	notifyf(who, "You equip %s.", eq->name);
	web_stats(who);
	web_content_out(who, eq);
	web_equipment(who);
	return 0;
}

void
do_select(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	const char *n_s = cmd->argv[1];
	unsigned n = strtoul(n_s, NULL, 0);
	eplayer->select = n;
	notifyf(player, "You select %u.", n);
}

void
do_equip(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	char const *name = cmd->argv[1];
	OBJ *eq = ematch_mine(player, name);

	if (!eq) {
		notify(player, "You are not carrying that.");
		return;
	}

	if (equip(player, eq)) { 
		notify(player, "You can't equip that.");
		return;
	}
}

dbref
unequip(OBJ *player, unsigned eql)
{
	ENT *eplayer = &player->sp.entity;
	dbref eq = EQUIP(eplayer, eql);
	unsigned eqt, aux;

	if (!eq)
		return NOTHING;

	OBJ *oeq = object_get(eq);
	EQU *eeq = &oeq->sp.equipment;
	eqt = EQT(eeq->eqw);
	aux = 0;

	switch (eql) {
	case ES_RHAND:
		EFFECT(eplayer, DMG).value -= DMG_WEAPON(eeq);
		eplayer->wts = phys_wts[eplayer->wtso];
		break;
	case ES_PANTS:
	case ES_HEAD:
		aux = 1;
	case ES_CHEST:
		switch (eqt) {
		case ARMOR_LIGHT: aux += 2; break;
		case ARMOR_MEDIUM: aux += 1; break;
		}
		aux = DEF_ARMOR(eeq, aux);
		EFFECT(eplayer, DEF).value -= aux;
		EFFECT(eplayer, DODGE).value += DODGE_ARMOR(aux);
	}

	EQUIP(eplayer, eql) = NOTHING;
	eeq->flags &= ~EF_EQUIPPED;
	web_content_in(player, oeq);
	web_stats(player);
	web_equipment(player);
	return eq;
}

void
do_unequip(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	char const *name = cmd->argv[1];
	enum bodypart bp = BODYPART_ID(*name);
	dbref eq;

	if ((eq = unequip(player, bp)) == NOTHING) {
		notify(player, "You don't have that equipped.");
		return;
	}

	notifyf(player, "You unequip %s.", object_get(eq)->name);
}
