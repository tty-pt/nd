#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "mcp.h"
#include "params.h"
#include "uapi/entity.h"
#include "uapi/io.h"
#include "uapi/match.h"
#include "uapi/wts.h"

void
notify_attack(unsigned player_ref, unsigned target_ref, char *wts, short val, char const *color, short mval)
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

	notify_wts_to(player_ref, target_ref, wts, wts_plural(wts), "%s", buf);
}


void
do_fight(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd);
	OBJ player, loc, target;
	lhash_get(obj_hd, &player, player_ref);
	unsigned target_ref = strcmp(argv[1], "me")
		? ematch_near(player_ref, argv[1])
		: player_ref;

	lhash_get(obj_hd, &loc, player.location);
	if (player.location == 0 || (loc.flags & RF_HAVEN)) {
		nd_writef(player_ref, "You may not fight here.\n");
		return;
	}

	lhash_get(obj_hd, &target, target_ref);
	if (target_ref == NOTHING
	    || player_ref == target_ref
	    || target.type != TYPE_ENTITY)
	{
		nd_writef(player_ref, "You can't target that.\n");
		return;
	}

	ENT eplayer = ent_get(player_ref);
	eplayer.target = target_ref;
	ent_set(player_ref, &eplayer);
	/* ndc_writef(fd, "You form a combat pose."); */
}

void
do_status(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	unsigned player_ref = fd_player(fd);
	ENT eplayer = ent_get(player_ref);
	// TODO optimize MOB_EV / MOB_EM
	nd_writef(player_ref, "hp %u/%u\tmp %u/%u\tstuck 0x%x\n"
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
do_heal(int fd, int argc __attribute__((unused)), char *argv[])
{
	char *name = argv[1];
	unsigned player_ref = fd_player(fd), target_ref;

	if (strcmp(name, "me")) {
		target_ref = ematch_near(player_ref, name);

	} else
		target_ref = player_ref;

	if (target_ref == NOTHING) {
                nd_writef(player_ref, "Invalid target\n");
		return;
	}

	OBJ player;
	lhash_get(obj_hd, &player, player_ref);

	if (!(ent_get(player_ref).flags & EF_WIZARD)
	    || player.type != TYPE_ENTITY) {
                nd_writef(player_ref, "Invalid target\n");
                return;
        }

	ENT etarget = ent_get(target_ref);
	etarget.hp = HP_MAX(&etarget);
	etarget.mp = MP_MAX(&etarget);
	etarget.huth[HUTH_THIRST] = etarget.huth[HUTH_HUNGER] = 0;
	debufs_end(&etarget);
	ent_set(target_ref, &etarget);
	notify_wts_to(player_ref, target_ref, "heal", "heals", "");
	mcp_bars(target_ref);
}

void
do_advitam(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd);
	char *name = argv[1];
	unsigned target_ref = ematch_near(player_ref, name);

	if (!(ent_get(player_ref).flags & EF_WIZARD) || target_ref == NOTHING) {
		nd_writef(player_ref, "You can't do that.\n");
		return;
	}

	OBJ target;
	lhash_get(obj_hd, &target, target_ref);

	if (target.owner != player_ref) {
		nd_writef(player_ref, "You don't own that.\n");
		return;
	}

	ENT etarget;
	birth(&etarget);
	ent_set(target_ref, &etarget);
	target.type = TYPE_ENTITY;
	lhash_put(obj_hd, target_ref, &target);
	nd_writef(player_ref, "You infuse %s with life.\n", target.name);
}

void
do_train(int fd, int argc __attribute__((unused)), char *argv[]) {
	unsigned player_ref = fd_player(fd);
	ENT eplayer = ent_get(player_ref);
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
		  nd_writef(player_ref, "Invalid attribute.\n");
		  return;
	}

	int avail = eplayer.spend;
	int amount = *amount_s ? atoi(amount_s) : 1;

	if (amount > avail) {
		  nd_writef(player_ref, "Not enough points.\n");
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
	ent_set(player_ref, &eplayer);
	nd_writef(player_ref, "Your %s increases %d time(s).\n", attrib, amount);
        mcp_stats(player_ref);
}

int
kill_v(unsigned player_ref, char const *opcs)
{
	ENT eplayer = ent_get(player_ref);
	char *end;
	if (isdigit(*opcs)) {
		unsigned combo = strtol(opcs, &end, 0);
		eplayer.combo = combo;
		ent_set(player_ref, &eplayer);
		nd_writef(player_ref, "Set combo to 0x%x.\n", combo);
		return end - opcs;
	} else if (*opcs == 'c' && isdigit(opcs[1])) {
		unsigned slot = strtol(opcs + 1, &end, 0);
		OBJ player;
		lhash_get(obj_hd, &player, player_ref);
		if (player.location == 0)
			nd_writef(player_ref, "You may not cast spells in room 0.\n");
		else {
			spell_cast(player_ref, &eplayer, eplayer.target, slot);
			ent_set(player_ref, &eplayer);
		}
		return end - opcs;
	} else
		return 0;
}

void
do_sit(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd);
	ENT eplayer = ent_get(player_ref);
        sit(player_ref, &eplayer, argv[1]);
	ent_set(player_ref, &eplayer);
}

void
do_stand(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	unsigned player_ref = fd_player(fd);
	ENT eplayer = ent_get(player_ref);
        stand(player_ref, &eplayer);
	ent_set(player_ref, &eplayer);
}
