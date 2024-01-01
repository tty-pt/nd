#include <ndc.h>
#include <stdlib.h>
#include <ctype.h>
#include "io.h"
#include "entity.h"
#include "debug.h"
#include "params.h"
#include "match.h"
#include "spell.h"

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
do_kill(int fd, int argc, char *argv[])
{
	char *what = argv[1];
	OBJ *player = FD_PLAYER(fd);
	OBJ *here = player->location;
	ROO *rhere = &here->sp.room;
	OBJ *target = strcmp(what, "me")
		? ematch_near(player, what)
		: player;

	if (object_ref(here) == 0 || (rhere->flags & RF_HAVEN)) {
		ndc_writef(fd, "You may not kill here.\n");
		return;
	}

	CBUG(player->type != TYPE_ENTITY);

	if (!target
	    || player == target
	    || target->type != TYPE_ENTITY)
	{
		ndc_writef(fd, "You can't target that.\n");
		return;
	}

	player->sp.entity.target = target;
	/* ndc_writef(fd, "You form a combat pose."); */
}

void
do_status(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	// TODO optimize MOB_EV / MOB_EM
	ndc_writef(fd, "hp %u/%u\tmp %u/%u\tstuck 0x%x\n"
		"dodge %d\tcombo 0x%x \tdebuf_mask 0x%x\n"
		"damage %d\tmdamage %d\tmdmg_mask 0x%x\n"
		"defense %d\tmdefense %d\tmdef_mask 0x%x\n"
		"klock   %u\thunger %u\tthirst %u\n",
		eplayer->hp, HP_MAX(eplayer), eplayer->mp, MP_MAX(eplayer), EFFECT(eplayer, MOV).mask,
		EFFECT(eplayer, DODGE).value, eplayer->combo, eplayer->debuf_mask,
		EFFECT(eplayer, DMG).value, EFFECT(eplayer, MDMG).value, EFFECT(eplayer, MDMG).mask,
		EFFECT(eplayer, DEF).value, EFFECT(eplayer, MDEF).value, EFFECT(eplayer, MDEF).mask,
		eplayer->klock, eplayer->hunger, eplayer->thirst);
}

void
do_heal(int fd, int argc, char *argv[])
{
	char *name = argv[1];
	OBJ *player = FD_PLAYER(fd),
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
                ndc_writef(fd, "You can't do that.\n");
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
do_advitam(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	char *name = argv[1];
	OBJ *target = ematch_near(player, name);

	if (!(eplayer->flags & EF_WIZARD)
	    || !target
	    || target->owner != player) {
		ndc_writef(fd, "You can't do that.\n");
		return;
	}

	birth(target);
	ndc_writef(fd, "You infuse %s with life.\n", target->name);
}

void
do_train(int fd, int argc, char *argv[]) {
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	const char *attrib = argv[1];
	const char *amount_s = argv[2];
	int attr;

	switch (attrib[0]) {
	case 's': attr = ATTR_STR; break;
	case 'c': attr = ATTR_CON; break;
	case 'd': attr = ATTR_DEX; break;
	case 'i': attr = ATTR_INT; break;
	case 'w': attr = ATTR_WIZ; break;
	case 'h': attr = ATTR_CHA; break;
	default:
		  ndc_writef(fd, "Invalid attribute.\n");
		  return;
	}

	int avail = eplayer->spend;
	int amount = *amount_s ? atoi(amount_s) : 1;

	if (amount > avail) {
		  ndc_writef(fd, "Not enough points.\n");
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
	ndc_writef(fd, "Your %s increases %d time(s).\n", attrib, amount);
        mcp_stats(player);
}

int
kill_v(OBJ *player, char const *opcs)
{
	ENT *eplayer = &player->sp.entity;
	int fd = eplayer->fd;
	char *end;
	if (isdigit(*opcs)) {
		unsigned combo = strtol(opcs, &end, 0);
		eplayer->combo = combo;
		ndc_writef(fd, "Set combo to 0x%x.\n", combo);
		return end - opcs;
	} else if (*opcs == 'c' && isdigit(opcs[1])) {
		unsigned slot = strtol(opcs + 1, &end, 0);
                OBJ *target = eplayer->target;
		if (player->location == 0) {
			ndc_writef(fd, "You may not cast spells in room 0.\n");
		} else {
			spell_cast(player, target, slot);
		}
		return end - opcs;
	} else
		return 0;
}

void
do_sit(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	char *name = argv[1];
        sit(player, name);
}

void
do_stand(int fd, int argc, char *argv[])
{
        stand(FD_PLAYER(fd));
}
