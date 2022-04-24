#include "kill.h"
#include <math.h>
#include "mdb.h"
#include "props.h"
#include "externs.h"
#include "item.h"
#include "params.h"
#include "geography.h"
#include "view.h"
#include "search.h"
#include "spell.h"
#include "mob.h"
#include "web.h"

#define MESGPROP_CXP	"_/cxp"
#define GETCXP(x)	get_property_value(x, MESGPROP_CXP)
#define SETCXP(x, y)	set_property_value(x, MESGPROP_CXP, y)

#define GETSPEND(x)	get_property_value(x, MESGPROP_STAT "/spend")
#define SETSPEND(x, y)	set_property_value(x, MESGPROP_STAT "/spend", y)

#define MESGPROP_SEATM	"_seat_m"
#define GETSEATM(x)	get_property_value(x, MESGPROP_SEATM)
/* #define SETSEATM(x,y)	set_property_value(x, MESGPROP_SEATM, y) */

#define MESGPROP_SEATN	"_seat_n"
#define GETSEATN(x)	get_property_value(x, MESGPROP_SEATN)
#define SETSEATN(x, y)	set_property_value(x, MESGPROP_SEATN, y)

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
dodge_get(struct mob *att)
{
	dbref target = att->target;
	struct mob *tar = MOB(target);
	int d = RAND_MAX / 4;
	short ad = MOB_EV(att, DODGE),
	      dd = MOB_EV(tar, DODGE);

	if (ad > dd)
		d += 3 * d / (ad - dd);

	return rand() <= d;
}

static inline unsigned
xp_get(unsigned x, unsigned y)
{
	// alternatively (2000/x)*y/x
	unsigned r = 254 * y / (x * x);
	if (r < 0)
		return 0;
	else
		return r;
}

int
kill_dodge(dbref attacker, struct wts wts)
{
	struct mob *att = MOB(attacker);
	dbref target = att->target;
	struct mob *tar = MOB(target);
	if (!MOB_EV(tar, MOV) && dodge_get(att)) {
		notify_wts_to(target, attacker, "dodge", "dodges", "'s %s", wts.a);
		return 1;
	} else
		return 0;
}

void
notify_attack(dbref att, dbref tar, struct wts wts, short val, char const *color, short mval)
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

	notify_wts_to(att, tar, wts.a, wts.b, "%s", buf);
}

static inline void
living_dead(dbref who)
{
	struct mob *mob = MOB(who);
	mob->target = NOTHING;
	mob->hp = HP_MAX(who);
	mob->mp = MP_MAX(who);
	mob->thirst = mob->hunger = 0;
	debufs_end(who);
}

static inline dbref
body(dbref player, dbref mob)
{
	char buf[32];
	struct object_skeleton o = { "", "", "" };
	snprintf(buf, sizeof(buf), "%s's body.", NAME(mob));
	o.name = buf;
	dbref dead_mob = object_add(o, getloc(mob));
	dbref tmp;
	unsigned n = 0;

	for (; (tmp = db[mob].contents) != NOTHING; ) {
		if (Typeof(tmp) == TYPE_GARBAGE)
			continue;
		unequip(mob, GETEQL(tmp));
		moveto(tmp, dead_mob);
		n++;
	}

	if (n > 0) {
		ONOTIFYF(mob, "%s's body drops to the ground.", NAME(mob));
		struct boolexp *key = parse_boolexp(player, NAME(player), 0);
		SETCONLOCK(dead_mob, key);
		return dead_mob;
	} else {
		recycle(player, dead_mob);
		return NOTHING;
	}
}

static inline void
_xp_award(dbref who, unsigned const xp)
{
	unsigned cxp = GETCXP(who);
	notifyf(who, "You gain %u xp!", xp);
	cxp += xp;
	while (cxp >= 1000) {
		notify(who, "You level up!");
		cxp -= 1000;
		SETLVL(who, GETLVL(who) + 1);
		SETSPEND(who, GETSPEND(who) + 2);
	}
	SETCXP(who, cxp);
}

static inline void
xp_award(dbref attacker, dbref target)
{
	unsigned x = GETLVL(attacker);
	unsigned y = GETLVL(target);
	_xp_award(attacker, xp_get(x, y));
}

void
kill_target(dbref attacker, dbref target)
{
	struct mob *att = attacker > 0 ? MOB(attacker) : NULL;
	struct mob *tar = MOB(target);

	notify_wts(target, "die", "dies", "");

	if (attacker > 0 && Typeof(attacker) == TYPE_PLAYER)
		xp_award(attacker, target);

	if (Typeof(target) == TYPE_PLAYER) {
		dbref loc = getloc(target);
		moveto(target, PLAYER_HOME(target));
		geo_clean(target, loc);
		tar->klock = 0;
                look_around(target);
                view(target);
	} else {
		if (tar->target && GETAGGRO(target)) {
			dbref target_target = tar->target;
			struct mob *tar_tar = MOB(target_target);
			tar_tar->klock --;
		}

		if (GETTMP(getloc(target))) {
			if (att) {
				body(attacker, target);
				att->target = NOTHING;
			}
			recycle(attacker, target);
			return;
		}

		moveto(target, (dbref) 0);
		tar->respawn_in = 30;
		tar->hp = HP_MAX(target);
		tar->mp = MP_MAX(target);
	}

	living_dead(target);

	if (att)
		att->target = NOTHING;
}

static inline void
attack(dbref attacker)
{
	struct mob *att = MOB(attacker),
		   *tar;

	register unsigned char mask;

	mask = MOB_EM(att, MOV);

	if (mask) {
		register unsigned i = __builtin_ffs(mask) - 1;
		debuf_notify(attacker, &att->debufs[i], 0);
		return;
	}

	dbref target = att->target;

	if (target <= 0)
		return;

	tar = MOB(target);

	if (!spells_cast(attacker, target) && !kill_dodge(attacker, att->wts)) {
		enum element at = ELEMENT_NEXT(attacker, MDMG);
		enum element dt = ELEMENT_NEXT(target, MDEF);
		short aval = -kill_dmg(ELM_PHYSICAL, MOB_EV(att, DMG), MOB_EV(tar, DEF) + MOB_EV(tar, MDEF), dt);
		short bval = -kill_dmg(at, MOB_EV(att, MDMG), MOB_EV(tar, MDEF), dt);
		char const *color = ELEMENT(at)->color;
		notify_attack(attacker, target, att->wts, aval, color, bval);
		cspell_heal(attacker, target, aval + bval);
	}
}

void
do_kill(command_t *cmd)
{
	int descr = cmd->fd;
	dbref player = cmd->player;
	const char *what = cmd->argv[1];
	dbref here = getloc(player);
	dbref target = strcmp(what, "me")
		? ematch_near(player, what)
		: player;
	struct mob *att, *tar;

        att = GETLID(player) ? MOB(player) : mob_put(player);

	if (FLAGS(here) & HAVEN
	    || target == NOTHING
	    || player == target
	    || !(tar = MOB(target)))
	{
		notify(player, "You can't target that.");
		return;
	}

	att->target = target;
	att->descr = descr;
	/* notify(player, "You form a combat pose."); */
}

void
kill_update(dbref attacker)
{
	struct mob *att = MOB(attacker);
	dbref target = att->target;

	if (target <= 0
            || Typeof(target) == TYPE_GARBAGE
	    || getloc(target) != getloc(attacker)) {
		att->target = NOTHING;
		return;
	}

	struct mob *tar = MOB(target);

	if (tar->target <= 0)
		tar->target = attacker;

	do_stand_silent(attacker);
	attack(attacker);
}

void
do_status(command_t *cmd)
{
	dbref player = cmd->player;
	// TODO optimize MOB_EV / MOB_EM
	struct mob *mob = MOB(player);
	notifyf(player, "hp %u/%u\tmp %u/%u\tstuck 0x%x\n"
		"dodge %d\tcombo 0x%x \tdebuf_mask 0x%x\n"
		"damage %d\tmdamage %d\tmdmg_mask 0x%x\n"
		"defense %d\tmdefense %d\tmdef_mask 0x%x\n"
		"klock   %u\thunger %u\tthirst %u",
		mob->hp, HP_MAX(player), mob->mp, MP_MAX(player), MOB_EM(mob, MOV),
		MOB_EV(mob, DODGE), mob->combo, mob->debuf_mask,
		MOB_EV(mob, DMG), MOB_EV(mob, MDMG), MOB_EM(mob, MDMG),
		MOB_EV(mob, DEF), MOB_EV(mob, MDEF), MOB_EM(mob, MDEF),
		mob->klock, mob->hunger, mob->thirst);
}

void
do_heal(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	dbref target;
	struct mob *tar;

	if (strcmp(name, "me")) {
		target = ematch_near(player, name);

	} else
		target = player;

	if (!(FLAGS(player) & WIZARD)
	    || target < 0
	    || GETLID(target) < 0) {
                notify(player, "You can't do that.");
                return;
        }

        tar = MOB(target);
	tar->hp = HP_MAX(target);
	tar->mp = MP_MAX(target);
	tar->hunger = tar->thirst = 0;
	debufs_end(target);
	notify_wts_to(player, target, "heal", "heals", "");
	web_bars(target);
}

void
do_advitam(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	dbref target = ematch_near(player, name);

	if (!(FLAGS(player) & WIZARD)
	    || target == NOTHING
	    || OWNER(target) != player) {
		notify(player, "You can't do that.");
		return;
	}

	mob_put(target);
	notifyf(player, "You infuse %s with life.", NAME(target));
}

void
do_givexp(command_t *cmd, const char *name, const char *amount)
{
	dbref player = cmd->player;
	dbref target = ematch_near(player, name);
	int amt = strtol(amount, NULL, 0);

	if (!(FLAGS(player) & WIZARD)
	    || target == NOTHING)
		notify(player, "You can't do that.");
	else
		_xp_award(target, amt);
}

void
do_train(command_t *cmd) {
	dbref player = cmd->player;
	const char *attrib = cmd->argv[1];
	const char *amount_s = cmd->argv[2];
	struct mob *mob = MOB(player);
	int attr;

	switch (attrib[0]) {
	case 's': attr = STR; break;
	case 'c': attr = CON; break;
	case 'd': attr = DEX; break;
	case 'i': attr = INT; break;
	case 'w': attr = WIZ; break;
	default:
		  notify(player, "Invalid attribute.");
		  return;
	}

	int avail = GETSPEND(player);
	int amount = *amount_s ? atoi(amount_s) : 1;

	if (amount > avail) {
		  notify(player, "Not enough points.");
		  return;
	}

	int c = GETSTAT(player, attr);
	SETSTAT(player, attr, c + amount);

	switch (attr) {
	case STR:
		MOB_EV(mob, DMG) += DMG_G(c + amount) - DMG_G(c);
		break;
	case DEX:
		MOB_EV(mob, DODGE) += DODGE_G(c + amount) - DODGE_G(c);
		break;
	}

	SETSPEND(player, avail - amount);
	notifyf(player, "Your %s increases %d time(s).", attrib, amount);
        web_stats(player);
}

int
kill_v(dbref player, char const *opcs)
{
	char *end;
	if (isdigit(*opcs)) {
		unsigned combo = strtol(opcs, &end, 0);
		SETCOMBO(player, combo);
		MOB(player)->combo = combo;
		notifyf(player, "Set combo to 0x%x.", combo);
		return end - opcs;
	} else if (*opcs == 'c' && isdigit(opcs[1])) {
		struct mob *p = MOB(player);
		unsigned slot = strtol(opcs + 1, &end, 0);
                dbref target = p->target == NOTHING ? player : p->target;
		spell_cast(player, target, slot);
		return end - opcs;
	} else
		return 0;
}

void
sit(dbref player, const char *name)
{
	if (GETSAT(player) != NOTHING) {
		notify(player, "You are already sitting.");
		return;
	}

	if (!*name) {
		notify_wts(player, "sit", "sits", " on the ground");
		SETSAT(player, 0);
		return;
	}

	dbref seat = ematch_near(player, name);
	int max = GETSEATM(seat);
	if (!max) {
		notify(player, "You can't sit on that.");
		return;
	}

	int cur = GETSEATN(seat);
	if (cur >= max) {
		notify(player, "No seats available.");
		return;
	}

	SETSEATN(seat, cur + 1);
	SETSAT(player, seat);
	MOB(player)->flags |= MF_SITTING;

	notify_wts(player, "sit", "sits", " on %s", NAME(seat));
}

void
do_sit(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
        sit(player, name);
}

int
do_stand_silent(dbref player)
{
	dbref chair = GETSAT(player);

	if (chair == NOTHING)
		return 1;

	if (chair > 0)
		SETSEATN(chair, GETSEATN(chair) - 1);

	USETSAT(player);
	MOB(player)->flags ^= MF_SITTING;
	notify_wts(player, "stand", "stands", " up");
	return 0;
}

void
stand(dbref player) {
	if (do_stand_silent(player))
		notify(player, "You are already standing.");
}

void
do_stand(command_t *cmd)
{
        stand(cmd->player);
}
