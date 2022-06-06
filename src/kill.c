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
dodge_get(struct entity *att)
{
	dbref target = att->target;
	struct entity *tar = ENTITY(target);
	int d = RAND_MAX / 4;
	short ad = EFFECT(att, DODGE).value,
	      dd = EFFECT(tar, DODGE).value;

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
	struct entity *att = ENTITY(attacker);
	dbref target = att->target;
	struct entity *tar = ENTITY(target);
	if (!EFFECT(tar, MOV).value && dodge_get(att)) {
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
	unsigned cxp = ENTITY(who)->cxp;
	notifyf(who, "You gain %u xp!", xp);
	cxp += xp;
	while (cxp >= 1000) {
		notify(who, "You level up!");
		cxp -= 1000;
		ENTITY(who)->lvl += 1;
		ENTITY(who)->spend += 2;
	}
	ENTITY(who)->cxp = cxp;
}

static inline void
xp_award(dbref attacker, dbref target)
{
	unsigned x = ENTITY(attacker)->lvl;
	unsigned y = ENTITY(target)->lvl;
	_xp_award(attacker, xp_get(x, y));
}

void
kill_target(dbref attacker, dbref target)
{
	struct entity *att = attacker > 0 ? ENTITY(attacker) : NULL;
	struct entity *tar = ENTITY(target);

	notify_wts(target, "die", "dies", "");

	body(attacker, target);

	if (attacker > 0) {
		CBUG(Typeof(attacker) != TYPE_ENTITY)
		xp_award(attacker, target);
		att->target = NOTHING;
	}

	CBUG(Typeof(target) != TYPE_ENTITY);

	if (tar->target && (ENTITY(target)->flags & EF_AGGRO)) {
		dbref target_target = tar->target;
		struct entity *tar_tar = ENTITY(target_target);
		tar_tar->klock --;
	}

	dbref loc = getloc(target);

	if ((ROOM(loc)->flags & RF_TEMP) && !(tar->flags & EF_PLAYER)) {
		recycle(attacker, target);
		geo_clean(target, loc);
		return;
	}

	tar->klock = 0;
	tar->target = NOTHING;
	tar->hp = 1;
	tar->mp = 1;
	tar->thirst = tar->hunger = 0;

	debufs_end(target);
	moveto(target, (dbref) 0);
	geo_clean(target, loc);
	look_around(target);
	web_bars(target);
}

static inline void
attack(dbref attacker)
{
	struct entity *att = ENTITY(attacker),
		      *tar;

	register unsigned char mask;

	mask = EFFECT(att, MOV).mask;

	if (mask) {
		register unsigned i = __builtin_ffs(mask) - 1;
		debuf_notify(attacker, &att->debufs[i], 0);
		return;
	}

	dbref target = att->target;

	if (target <= 0)
		return;

	tar = ENTITY(target);

	if (!spells_cast(attacker, target) && !kill_dodge(attacker, att->wts)) {
		enum element at = ELEMENT_NEXT(attacker, MDMG);
		enum element dt = ELEMENT_NEXT(target, MDEF);
		short aval = -kill_dmg(ELM_PHYSICAL, EFFECT(att, DMG).value, EFFECT(tar, DEF).value + EFFECT(tar, MDEF).value, dt);
		short bval = -kill_dmg(at, EFFECT(att, MDMG).value, EFFECT(tar, MDEF).value, dt);
		char const *color = ELEMENT(at)->color;
		notify_attack(attacker, target, att->wts, aval, color, bval);
		cspell_heal(attacker, target, aval + bval);
	}
}

void
do_kill(command_t *cmd)
{
	dbref player = cmd->player;
	const char *what = cmd->argv[1];
	dbref here = getloc(player);
	dbref target = strcmp(what, "me")
		? ematch_near(player, what)
		: player;

	if (here == 0) {
		notify(player, "You may not kill in room 0");
		return;
	}

	CBUG(Typeof(player) != TYPE_ENTITY);
	struct entity *att = ENTITY(player);

	if (FLAGS(here) & HAVEN
	    || target == NOTHING
	    || player == target
	    || Typeof(target) != TYPE_ENTITY)
	{
		notify(player, "You can't target that.");
		return;
	}

	att->target = target;
	/* notify(player, "You form a combat pose."); */
}

void
kill_update(dbref attacker)
{
	struct entity *att = ENTITY(attacker);
	dbref target = att->target;

	if (target <= 0
            || Typeof(target) == TYPE_GARBAGE
	    || getloc(target) != getloc(attacker)) {
		att->target = NOTHING;
		return;
	}

	struct entity *tar = ENTITY(target);

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
	struct entity *mob = ENTITY(player);
	notifyf(player, "hp %u/%u\tmp %u/%u\tstuck 0x%x\n"
		"dodge %d\tcombo 0x%x \tdebuf_mask 0x%x\n"
		"damage %d\tmdamage %d\tmdmg_mask 0x%x\n"
		"defense %d\tmdefense %d\tmdef_mask 0x%x\n"
		"klock   %u\thunger %u\tthirst %u",
		mob->hp, HP_MAX(player), mob->mp, MP_MAX(player), EFFECT(mob, MOV).mask,
		EFFECT(mob, DODGE).value, mob->combo, mob->debuf_mask,
		EFFECT(mob, DMG).value, EFFECT(mob, MDMG).value, EFFECT(mob, MDMG).mask,
		EFFECT(mob, DEF).value, EFFECT(mob, MDEF).value, EFFECT(mob, MDEF).mask,
		mob->klock, mob->hunger, mob->thirst);
}

void
do_heal(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	dbref target;
	struct entity *tar;

	if (strcmp(name, "me")) {
		target = ematch_near(player, name);

	} else
		target = player;

	if (!(FLAGS(player) & WIZARD)
	    || target < 0
	    || Typeof(target) != TYPE_ENTITY) {
                notify(player, "You can't do that.");
                return;
        }

        tar = ENTITY(target);
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

	birth(target);
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
	struct entity *mob = ENTITY(player);
	int attr;

	switch (attrib[0]) {
	case 's': attr = ATTR_STR; break;
	case 'c': attr = ATTR_CON; break;
	case 'd': attr = ATTR_DEX; break;
	case 'i': attr = ATTR_INT; break;
	case 'w': attr = ATTR_WIZ; break;
	default:
		  notify(player, "Invalid attribute.");
		  return;
	}

	int avail = ENTITY(player)->spend;
	int amount = *amount_s ? atoi(amount_s) : 1;

	if (amount > avail) {
		  notify(player, "Not enough points.");
		  return;
	}

	unsigned c = ATTR(player, attr);
	ATTR(player, attr) += amount;

	switch (attr) {
	case ATTR_STR:
		EFFECT(mob, DMG).value += DMG_G(c + amount) - DMG_G(c);
		break;
	case ATTR_DEX:
		EFFECT(mob, DODGE).value += DODGE_G(c + amount) - DODGE_G(c);
		break;
	}

	ENTITY(player)->spend = avail - amount;
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
		ENTITY(player)->combo = combo;
		notifyf(player, "Set combo to 0x%x.", combo);
		return end - opcs;
	} else if (*opcs == 'c' && isdigit(opcs[1])) {
		struct entity *p = ENTITY(player);
		unsigned slot = strtol(opcs + 1, &end, 0);
                dbref target = p->target == NOTHING ? player : p->target;
		if (getloc(player) == 0) {
			notify(player, "You may not cast spells in room 0");
		} else {
			spell_cast(player, target, slot);
		}
		return end - opcs;
	} else
		return 0;
}

void
sit(dbref player, const char *name)
{
	if (ENTITY(player)->flags & EF_SITTING) {
		notify(player, "You are already sitting.");
		return;
	}

	if (!*name) {
		notify_wts(player, "sit", "sits", " on the ground");
		ENTITY(player)->flags |= EF_SITTING;
		ENTITY(player)->sat = -1;

		/* warn("%d sits on the ground\n", player); */
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
	ENTITY(player)->flags |= EF_SITTING;
	ENTITY(player)->sat = seat;

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
	if (!(ENTITY(player)->flags & EF_SITTING))
		return 1;

	dbref chair = ENTITY(player)->sat;

	if (chair > 0) {
		SETSEATN(chair, GETSEATN(chair) - 1);
		ENTITY(player)->sat = -1;
	}

	ENTITY(player)->flags ^= EF_SITTING;
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
