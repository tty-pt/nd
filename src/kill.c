#include <ndc.h>
#include <stdlib.h>
#include <ctype.h>
#include "io.h"
#include "entity.h"
#include "debug.h"
#include "params.h"
#include "match.h"
#include "spell.h"
#include "uapi/wts.h"

void
notify_attack(OBJ *player, OBJ *target, char *wts, short val, char const *color, short mval)
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

	notify_wts_to(player, target, wts, wts_plural(wts), "%s", buf);
}


void
do_kill(int fd, int argc, char *argv[])
{
	char *what = argv[1];
	OBJ *player = FD_PLAYER(fd);
	OBJ *here = object_get(player->location);
	ROO *rhere = &here->sp.room;
	OBJ *target = strcmp(what, "me")
		? ematch_near(player, what)
		: player;

	if (object_ref(here) == 0 || (rhere->flags & RF_HAVEN)) {
		nd_writef(player, "You may not kill here.\n");
		return;
	}

	CBUG(player->type != TYPE_ENTITY);

	if (!target
	    || player == target
	    || target->type != TYPE_ENTITY)
	{
		nd_writef(player, "You can't target that.\n");
		return;
	}

	ENT_SETF(object_ref(player), target, object_ref(target));
	/* ndc_writef(fd, "You form a combat pose."); */
}

void
do_status(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	ENT eplayer = ent_get(object_ref(player));
	// TODO optimize MOB_EV / MOB_EM
	nd_writef(player, "hp %u/%u\tmp %u/%u\tstuck 0x%x\n"
		"dodge %d\tcombo 0x%x \tdebuf_mask 0x%x\n"
		"damage %d\tmdamage %d\tmdmg_mask 0x%x\n"
		"defense %d\tmdefense %d\tmdef_mask 0x%x\n"
		"klock   %u\thunger %u\tthirst %u\n",
		eplayer.hp, HP_MAX(&eplayer), eplayer.mp, MP_MAX(&eplayer), EFFECT(&eplayer, MOV).mask,
		EFFECT(&eplayer, DODGE).value, eplayer.combo, eplayer.debuf_mask,
		EFFECT(&eplayer, DMG).value, EFFECT(&eplayer, MDMG).value, EFFECT(&eplayer, MDMG).mask,
		EFFECT(&eplayer, DEF).value, EFFECT(&eplayer, MDEF).value, EFFECT(&eplayer, MDEF).mask,
		eplayer.klock, eplayer.huth[HUTH_HUNGER], eplayer.huth[HUTH_THIRST]);
}

void
do_heal(int fd, int argc, char *argv[])
{
	char *name = argv[1];
	OBJ *player = FD_PLAYER(fd),
	    *target;

	if (strcmp(name, "me")) {
		target = ematch_near(player, name);

	} else
		target = player;

	if (!(ent_get(object_ref(player)).flags & EF_WIZARD)
	    || !target
	    || target->type != TYPE_ENTITY) {
                nd_writef(player, "You can't do that.\n");
                return;
        }

	ENT etarget = ent_get(object_ref(target));
	etarget.hp = HP_MAX(&etarget);
	etarget.mp = MP_MAX(&etarget);
	etarget.huth[HUTH_THIRST] = etarget.huth[HUTH_HUNGER] = 0;
	debufs_end(&etarget);
	ent_set(object_ref(target), &etarget);
	notify_wts_to(player, target, "heal", "heals", "");
	mcp_bars(target);
}

void
do_advitam(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	char *name = argv[1];
	OBJ *target = ematch_near(player, name);

	if (!(ent_get(object_ref(player)).flags & EF_WIZARD)
	    || !target
	    || target->owner != object_ref(player)) {
		nd_writef(player, "You can't do that.\n");
		return;
	}

	ENT etarget;
	birth(&etarget);
	ent_set(object_ref(target), &etarget);
	nd_writef(player, "You infuse %s with life.\n", target->name);
}

void
do_train(int fd, int argc, char *argv[]) {
	OBJ *player = FD_PLAYER(fd);
	ENT eplayer = ent_get(object_ref(player));
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
		  nd_writef(player, "Invalid attribute.\n");
		  return;
	}

	int avail = eplayer.spend;
	int amount = *amount_s ? atoi(amount_s) : 1;

	if (amount > avail) {
		  nd_writef(player, "Not enough points.\n");
		  return;
	}

	unsigned c = eplayer.attr[attr];
	eplayer.attr[attr] += amount;

	switch (attr) {
	case ATTR_STR:
		EFFECT(&eplayer, DMG).value += DMG_G(c + amount) - DMG_G(c);
		break;
	case ATTR_DEX:
		EFFECT(&eplayer, DODGE).value += DODGE_G(c + amount) - DODGE_G(c);
		break;
	}

	eplayer.spend = avail - amount;
	ent_set(object_ref(player), &eplayer);
	nd_writef(player, "Your %s increases %d time(s).\n", attrib, amount);
        mcp_stats(player);
}

int
kill_v(OBJ *player, char const *opcs)
{
	ENT eplayer = ent_get(object_ref(player));
	char *end;
	if (isdigit(*opcs)) {
		unsigned combo = strtol(opcs, &end, 0);
		eplayer.combo = combo;
		ent_set(object_ref(player), &eplayer);
		nd_writef(player, "Set combo to 0x%x.\n", combo);
		return end - opcs;
	} else if (*opcs == 'c' && isdigit(opcs[1])) {
		unsigned slot = strtol(opcs + 1, &end, 0);
                OBJ *target = object_get(eplayer.target);
		if (player->location == 0) {
			nd_writef(player, "You may not cast spells in room 0.\n");
		} else {
			ENT etarget = ent_get(eplayer.target);
			spell_cast(player, &eplayer, target, &etarget, slot);
			ent_set(object_ref(player), &eplayer);
			ent_set(object_ref(target), &etarget);
		}
		return end - opcs;
	} else
		return 0;
}

void
do_sit(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	int ref = object_ref(player);
	ENT eplayer = ent_get(ref);
	char *name = argv[1];
        sit(player, &eplayer, name);
	ent_set(ref, &eplayer);
}

void
do_stand(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	int ref = object_ref(player);
	ENT eplayer = ent_get(ref);
        stand(player, &eplayer);
	ent_set(ref, &eplayer);
}
