#include "io.h"
#include "entity.h"
#include "kill.h"
#include <math.h>
#include "mdb.h"
#include "props.h"
#include "externs.h"
#include "item.h"
#include "params.h"
#include "geography.h"
#include "view.h"
#include "spell.h"
#include "mob.h"

struct wts phys_wts[] = {
	{ "punch", "punches" },
	{ "peck", "pecks at" },
	{ "slash", "slashes" },
	{ "bite", "bites" },
	{ "strike", "strikes" },
};

static inline short
randd_dmg(short dmg)
{
	register unsigned short xx = 1 + (random() & 7);
	return xx = dmg + ((dmg * xx * xx * xx) >> 9);
}

short
kill_dmg(enum element dmg_type, short dmg,
	short def, enum element def_type)
{
	if (dmg > 0) {
		if (dmg_type == ELEMENT(def_type)->weakness)
			dmg *= 2;
		else if (ELEMENT(dmg_type)->weakness == def_type)
			dmg /= 2;

		if (dmg < def)
			return 0;

	} else
		// heal TODO make type matter
		def = 0;

	return randd_dmg(dmg - def);
}

// returns 1 if target dodges
static inline int
dodge_get(ENT *eplayer)
{
	OBJ *target = eplayer->target;
	ENT *etarget = &target->sp.entity;
	int d = RAND_MAX / 4;
	short ad = EFFECT(eplayer, DODGE).value,
	      dd = EFFECT(etarget, DODGE).value;

	if (ad > dd)
		d += 3 * d / (ad - dd);

	return rand() <= d;
}

int
kill_dodge(OBJ *player, struct wts wts)
{
	ENT *eplayer = &player->sp.entity;
	OBJ *target = eplayer->target;
	ENT *etarget = &target->sp.entity;
	if (!EFFECT(etarget, MOV).value && dodge_get(eplayer)) {
		notify_wts_to(target, player, "dodge", "dodges", "'s %s", wts.a);
		return 1;
	} else
		return 0;
}

void
notify_attack(OBJ *player, OBJ *target, struct wts wts, short val, char const *color, short mval)
{
	char buf[BUFSIZ];
	unsigned i = 0;

	if (val || mval) {
		buf[i++] = ' ';
		buf[i++] = '(';

		if (val)
			i += snprintf(&buf[i], sizeof(buf) - i, "%d%s", val, mval ? ", " : "");

		if (mval)
			i += snprintf(&buf[i], sizeof(buf) - i, "%s%d%s", color, mval, ANSI_RESET);

		buf[i++] = ')';
	}
	buf[i] = '\0';

	notify_wts_to(player, target, wts.a, wts.b, "%s", buf);
}

static inline void
attack(OBJ *player)
{
	ENT *eplayer = &player->sp.entity,
	    *etarget;

	register unsigned char mask;

	mask = EFFECT(eplayer, MOV).mask;

	if (mask) {
		register unsigned i = __builtin_ffs(mask) - 1;
		debuf_notify(player, &eplayer->debufs[i], 0);
		return;
	}

	OBJ *target = eplayer->target;

	if (!target)
		return;

	etarget = &target->sp.entity;

	if (!spells_cast(player, target) && !kill_dodge(player, eplayer->wts)) {
		enum element at = ELEMENT_NEXT(eplayer, MDMG);
		enum element dt = ELEMENT_NEXT(etarget, MDEF);
		short aval = -kill_dmg(ELM_PHYSICAL, EFFECT(eplayer, DMG).value, EFFECT(etarget, DEF).value + EFFECT(etarget, MDEF).value, dt);
		short bval = -kill_dmg(at, EFFECT(eplayer, MDMG).value, EFFECT(etarget, MDEF).value, dt);
		char const *color = ELEMENT(at)->color;
		notify_attack(player, target, eplayer->wts, aval, color, bval);
		entity_damage(player, target, aval + bval);
	}
}

void
do_kill(command_t *cmd)
{
	const char *what = cmd->argv[1];
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	OBJ *here = player->location;
	ROO *rhere = &here->sp.room;
	OBJ *target = strcmp(what, "me")
		? ematch_near(player, what)
		: player;

	if (object_ref(here) == 0 || (rhere->flags & RF_HAVEN)) {
		notify(eplayer, "You may not kill here");
		return;
	}

	CBUG(player->type != TYPE_ENTITY);

	if (!target
	    || player == target
	    || target->type != TYPE_ENTITY)
	{
		notify(eplayer, "You can't target that.");
		return;
	}

	player->sp.entity.target = target;
	/* notify(eplayer, "You form a combat pose."); */
}

void
kill_update(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
	OBJ *target = eplayer->target;

	if (!target
            || target->type == TYPE_GARBAGE
	    || target->location != player->location) {
		eplayer->target = NULL;
		return;
	}

	ENT *etarget = &target->sp.entity;

	if (!etarget->target)
		etarget->target = player;

	stand_silent(player);
	attack(player);
}

void
do_status(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	// TODO optimize MOB_EV / MOB_EM
	notifyf(eplayer, "hp %u/%u\tmp %u/%u\tstuck 0x%x\n"
		"dodge %d\tcombo 0x%x \tdebuf_mask 0x%x\n"
		"damage %d\tmdamage %d\tmdmg_mask 0x%x\n"
		"defense %d\tmdefense %d\tmdef_mask 0x%x\n"
		"klock   %u\thunger %u\tthirst %u",
		eplayer->hp, HP_MAX(eplayer), eplayer->mp, MP_MAX(eplayer), EFFECT(eplayer, MOV).mask,
		EFFECT(eplayer, DODGE).value, eplayer->combo, eplayer->debuf_mask,
		EFFECT(eplayer, DMG).value, EFFECT(eplayer, MDMG).value, EFFECT(eplayer, MDMG).mask,
		EFFECT(eplayer, DEF).value, EFFECT(eplayer, MDEF).value, EFFECT(eplayer, MDEF).mask,
		eplayer->klock, eplayer->hunger, eplayer->thirst);
}

void
do_heal(command_t *cmd)
{
	const char *name = cmd->argv[1];
	OBJ *player = object_get(cmd->player),
	    *target;
	ENT *eplayer = &player->sp.entity,
	    *etarget;

	if (strcmp(name, "me")) {
		target = ematch_near(player, name);

	} else
		target = player;

	if (!(eplayer->flags & EF_WIZARD)
	    || !target
	    || target->type != TYPE_ENTITY) {
                notify(eplayer, "You can't do that.");
                return;
        }

	etarget = &target->sp.entity;
	etarget->hp = HP_MAX(etarget);
	etarget->mp = MP_MAX(etarget);
	etarget->hunger = etarget->thirst = 0;
	debufs_end(etarget);
	notify_wts_to(player, target, "heal", "heals", "");
	mcp_bars(etarget);
}

void
do_advitam(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *target = ematch_near(player, name);

	if (!(eplayer->flags & EF_WIZARD)
	    || !target
	    || target->owner != player) {
		notify(eplayer, "You can't do that.");
		return;
	}

	birth(target);
	notifyf(eplayer, "You infuse %s with life.", target->name);
}

void
do_train(command_t *cmd) {
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	const char *attrib = cmd->argv[1];
	const char *amount_s = cmd->argv[2];
	int attr;

	switch (attrib[0]) {
	case 's': attr = ATTR_STR; break;
	case 'c': attr = ATTR_CON; break;
	case 'd': attr = ATTR_DEX; break;
	case 'i': attr = ATTR_INT; break;
	case 'w': attr = ATTR_WIZ; break;
	default:
		  notify(eplayer, "Invalid attribute.");
		  return;
	}

	int avail = eplayer->spend;
	int amount = *amount_s ? atoi(amount_s) : 1;

	if (amount > avail) {
		  notify(eplayer, "Not enough points.");
		  return;
	}

	unsigned c = eplayer->attr[attr];
	eplayer->attr[attr] += amount;

	switch (attr) {
	case ATTR_STR:
		EFFECT(eplayer, DMG).value += DMG_G(c + amount) - DMG_G(c);
		break;
	case ATTR_DEX:
		EFFECT(eplayer, DODGE).value += DODGE_G(c + amount) - DODGE_G(c);
		break;
	}

	eplayer->spend = avail - amount;
	notifyf(eplayer, "Your %s increases %d time(s).", attrib, amount);
        mcp_stats(player);
}

int
kill_v(OBJ *player, char const *opcs)
{
	ENT *eplayer = &player->sp.entity;
	char *end;
	if (isdigit(*opcs)) {
		unsigned combo = strtol(opcs, &end, 0);
		eplayer->combo = combo;
		notifyf(eplayer, "Set combo to 0x%x.", combo);
		return end - opcs;
	} else if (*opcs == 'c' && isdigit(opcs[1])) {
		unsigned slot = strtol(opcs + 1, &end, 0);
                OBJ *target = eplayer->target;
		if (player->location == 0) {
			notify(eplayer, "You may not cast spells in room 0");
		} else {
			spell_cast(player, target, slot);
		}
		return end - opcs;
	} else
		return 0;
}

void
do_sit(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	const char *name = cmd->argv[1];
        sit(player, name);
}

void
stand(OBJ *player) {
	ENT *eplayer = &player->sp.entity;
	if (stand_silent(player))
		notify(eplayer, "You are already standing.");
}

void
do_stand(command_t *cmd)
{
        stand(object_get(cmd->player));
}
