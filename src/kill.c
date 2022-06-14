#include "io.h"
#include "entity.h"
#include <math.h>
#include "mdb.h"
#include "props.h"
#include "externs.h"
#include "params.h"
#include "view.h"
#include "spell.h"
#include "mob.h"

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


void
do_kill(command_t *cmd)
{
	const char *what = cmd->argv[1];
	OBJ *player = cmd->player;
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
do_status(command_t *cmd)
{
	OBJ *player = cmd->player;
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
	OBJ *player = cmd->player,
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
	OBJ *player = cmd->player;
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
	OBJ *player = cmd->player;
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
	OBJ *player = cmd->player;
	const char *name = cmd->argv[1];
        sit(player, name);
}

void
do_stand(command_t *cmd)
{
        stand(cmd->player);
}
