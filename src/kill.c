#include "kill.h"
#include <math.h>
#include "db.h"
#include "props.h"
#include "externs.h"
#include "item.h"
#include "params.h"
#include "geography.h"
#include "view.h"
#include "search.h"
#include "spell.h"
#include "mob.h"

#include "debug.h"

#define MESGPROP_CXP	"_/cxp"
#define GETCXP(x)	get_property_value(x, MESGPROP_CXP)
#define SETCXP(x, y)	set_property_value(x, MESGPROP_CXP, y)

#define GETSPEND(x)	get_property_value(x, MESGPROP_STAT "/spend")
#define SETSPEND(x, y)	set_property_value(x, MESGPROP_STAT "/spend", y)

#define MESGPROP_SAT	"_/sat"
#define GETSAT(x)	get_property_dbref(x, MESGPROP_SAT)
#define SETSAT(x,y)	set_property_dbref(x, MESGPROP_SAT, y)
#define USETSAT(x)	remove_property(x, MESGPROP_SAT)

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
dodge_get(mobi_t *att)
{
	int d = RAND_MAX / 4;
	short ad = MOBI_EV(att, DODGE),
	      dd = MOBI_EV(att->target, DODGE);

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
kill_dodge(mobi_t *att, struct wts wts)
{
	mobi_t *tar = att->target;
	if (!MOBI_EV(tar, MOV) && dodge_get(att)) {
		notify_wts_to(tar->who, att->who, "dodge", "dodges", "'s %s", wts.a);
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
living_dead(mobi_t *liv)
{
	liv->target = NULL;
	liv->hp = HP_MAX(liv->who);
	liv->mp = MP_MAX(liv->who);
	liv->thirst = liv->hunger = 0;
	debufs_end(liv->who);
}

static inline dbref
body(int descr, dbref player, dbref mob)
{
	char buf[32];
	struct obj o = { "", "", "" };
	snprintf(buf, sizeof(buf), "%s's body.", NAME(mob));
	o.name = buf;
	dbref dead_mob = obj_add(o, getloc(mob));
	dbref tmp;
	unsigned n = 0;

	for (; (tmp = DBFETCH(mob)->contents) != NOTHING; ) {
		if (Typeof(tmp) == TYPE_GARBAGE)
			continue;
		unequip(mob, GETEQL(tmp));
		moveto(tmp, dead_mob);
		n++;
	}

	if (n > 0) {
		ONOTIFYF(mob, "%s's body drops to the ground.", NAME(mob));
		struct boolexp *key = parse_boolexp(descr, player, NAME(player), 0);
		SETCONLOCK(dead_mob, key);
		return dead_mob;
	} else {
		recycle(descr, player, dead_mob);
		return NOTHING;
	}
}

static inline void
_xp_award(dbref who, unsigned const xp)
{
	unsigned cxp = GETCXP(who);
	notify_fmt(who, "You gain %u xp!", xp);
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
	mobi_t *att = attacker > 0 ? MOBI(attacker) : NULL;
	mobi_t *tar = MOBI(target);

	notify_wts(target, "die", "dies", "");

	if (attacker > 0 && Typeof(attacker) == TYPE_PLAYER)
		xp_award(attacker, target);

	if (Typeof(target) == TYPE_PLAYER) {
		dbref loc = getloc(target);
		moveto(target, PLAYER_HOME(target));
		geo_clean(tar->descr, target, loc);
		tar->klock = 0;
		do_view(tar->descr, target);
	} else {
		if (tar->target && GETAGGRO(target))
			tar->target->klock --;

		if (GETTMP(getloc(target))) {
			int descr = 0;
			if (att) {
				descr = att->descr;
				body(descr, attacker, target);
				att->target = NULL;
			}
			recycle(descr, attacker, target);
			tar->who = 0;
			return;
		}

		moveto(target, (dbref) 0);
		tar->respawn_in = 30;
		tar->hp = HP_MAX(target);
		tar->mp = MP_MAX(target);
	}

	living_dead(tar);

	if (att)
		att->target = NULL;
}

static inline void
attack(mobi_t *p, int player_attack)
{
	mobi_t *att, *tar;
	dbref attacker;
	register unsigned char mask;

	if (player_attack) {
		att = p;
		tar = p->target;
	} else {
		att = p->target;
		tar = p;
	}

	attacker = att->who;

	mask = MOBI_EM(att, MOV);

	if (mask) {
		register unsigned i = __builtin_ffs(mask) - 1;
		debuf_notify(attacker, &att->debufs[i], 0);
	} else if (!spells_cast(attacker, tar->who) && !kill_dodge(att, att->wts)) {
		enum element at = ELEMENT_NEXT(att->who, MDMG);
		enum element dt = ELEMENT_NEXT(tar->who, MDEF);
		dbref target = tar->who;
		short aval = -kill_dmg(ELM_PHYSICAL, MOBI_EV(att, DMG), MOBI_EV(tar, DEF) + MOBI_EV(tar, MDEF), dt);
		short bval = -kill_dmg(at, MOBI_EV(att, MDMG), MOBI_EV(tar, MDEF), dt);
		char const *color = ELEMENT(at)->color;
		notify_attack(attacker, target, att->wts, aval, color, bval);
		cspell_heal(attacker, target, aval + bval);
	}
}

static inline void
target_try(mobi_t *me, dbref target)
{
	mobi_t *tar;

	if (me->target)
		return;

	tar = MOBI(target);

	if (tar == me->target)
		return;

	me->target = tar;
}

void
do_kill(int descr, dbref player, const char *what)
{
	dbref here = getloc(player);
	dbref target = strcmp(what, "me")
		? contents_find(descr, player, here, what)
		: player;
	mobi_t *att, *tar;

        att = GETLID(player) ? MOBI(player) : mob_put(player);

	if (FLAGS(here) & HAVEN
	    || target == NOTHING
	    || player == target
#if RESTRICT_KILL
	    || ( Typeof(target) == TYPE_PLAYER
		 && !((FLAGS(target) & KILL_OK)
		      && (FLAGS(player) & KILL_OK)))
#endif
	    || !(tar = MOBI(target)))
	{
		notify(player, "You can't target that.");
		return;
	}

	// this happens when we clone living ones
	if (tar->who != target)
		tar = mob_put(target);

	do_stand_silent(player);
	att->target = tar;
	att->descr = descr;
	notify(player, "You form a combat pose.");
}

void
kill_update(mobi_t *liv)
{
	mobi_t *livt = liv->target;

	debug("kill_update %p %p\n", (void *) liv, (void *) livt);

	if (!livt
            || livt == liv
            || livt->who < 0
            || Typeof(livt->who) == TYPE_GARBAGE
	    || getloc(livt->who) != getloc(liv->who)) {
		liv->target = NULL;
		return;
	}

	target_try(liv->target, liv->who);
	attack(liv, 1);
}

void
do_status(dbref player)
{
	// TODO optimize MOBI_EV / MOBI_EM
	mobi_t *liv = MOBI(player);
	notify_fmt(player, "hp %u/%u\tmp %u/%u\tstuck 0x%x\n"
		   "dodge %d\tcombo 0x%x \tdebuf_mask 0x%x\n"
		   "damage %d\tmdamage %d\tmdmg_mask 0x%x\n"
		   "defense %d\tmdefense %d\tmdef_mask 0x%x\n"
		   "hunger %u\tthirst %u",
		   liv->hp, HP_MAX(player), liv->mp, MP_MAX(player), MOBI_EM(liv, MOV),
		   MOBI_EV(liv, DODGE), liv->combo, liv->debuf_mask,
		   MOBI_EV(liv, DMG), MOBI_EV(liv, MDMG), MOBI_EM(liv, MDMG),
		   MOBI_EV(liv, DEF), MOBI_EV(liv, MDEF), MOBI_EM(liv, MDEF),
		   liv->hunger, liv->thirst);
}

void
do_heal(int descr, dbref player, const char *name)
{
	dbref here = getloc(player);
	dbref target;
	mobi_t *tar;

	if (strcmp(name, "me")) {
		target = contents_find(descr, player, here, name);

	} else
		target = player;

	if (!(FLAGS(player) & WIZARD)
	    || target < 0
	    || GETLID(target) < 0) {
                notify(player, "You can't do that.");
                return;
        }

        tar = MOBI(target);
	tar->hp = HP_MAX(target);
	tar->mp = MP_MAX(target);
	tar->hunger = tar->thirst = 0;
	debufs_end(target);
	notify_wts_to(player, target, "heal", "heals", "");
}

void
do_advitam(int descr, dbref player, const char *name)
{
	dbref here = getloc(player);
	dbref target = contents_find(descr, player, here, name);

	if (!(FLAGS(player) & WIZARD)
	    || target == NOTHING
	    || MOBI(target)
	    || OWNER(target) != player) {
		notify(player, "You can't do that.");
		return;
	}

	mob_put(target);
	notify_fmt(player, "You infuse %s with life.", NAME(target));
}

void
do_givexp(int descr, dbref player, const char *name, const char *amount)
{
	dbref target = contents_find(descr, player, getloc(player), name);
	int amt = strtol(amount, NULL, 0);

	if (!(FLAGS(player) & WIZARD)
	    || target == NOTHING)
		notify(player, "You can't do that.");
	else
		_xp_award(target, amt);
}

void
do_train(dbref player, const char *attrib, const char *amount_s)
{
	mobi_t *liv = MOBI(player);
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
		MOBI_EV(liv, DMG) += DMG_G(c + amount) - DMG_G(c);
		break;
	case DEX:
		MOBI_EV(liv, DODGE) += DODGE_G(c + amount) - DODGE_G(c);
		break;
	}

	SETSPEND(player, avail - amount);
	notify_fmt(player, "Your %s increases %d time(s).", attrib, amount);
}

int
kill_v(int descr, dbref player, char const *opcs)
{
	char *end;
	if (isdigit(*opcs)) {
		unsigned combo = strtol(opcs, &end, 0);
		SETCOMBO(player, combo);
		MOBI(player)->combo = combo;
		notify_fmt(player, "Set combo to 0x%x.", combo);
		return end - opcs;
	} else if (*opcs == 'c' && isdigit(opcs[1])) {
		mobi_t *p = MOBI(player);
		unsigned slot = strtol(opcs + 1, &end, 0);
		CBUG(!p);

		if (!p->target)
			p->target = p;

		spell_cast(p->who, p->target && p->target->who, slot);
		return end - opcs;
	} else
		return 0;
}

void
do_sit(int descr, dbref player, const char *name)
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

	dbref seat = contents_find(descr, player, getloc(player), name);
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
	MOBI(player)->flags |= MF_SITTING;

	notify_wts(player, "sit", "sits", " on %s", NAME(seat));
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
	MOBI(player)->flags ^= MF_SITTING;
	notify_wts(player, "stand", "stands", " up");
	return 0;
}

void
do_stand(dbref player)
{
	if (do_stand_silent(player))
		notify(player, "You are already standing.");
}
