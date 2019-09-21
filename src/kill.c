#include "kill.h"
#include <math.h>
#include "db.h"
#include "props.h"
#include "externs.h"
#include "item.h"
#include "params.h"
#include "geography.h"
#include "debug.h"

#define DMG_BASE(p) DMG_G(GETSTAT(p, STR))
#define DODGE_BASE(p) DODGE_G(GETSTAT(p, DEX))

#define HP_G(v) 10 * G(v)
#define HP_MAX(p) HP_G(GETSTAT(p, CON))

#define MP_G(v) HP_G(v)
#define MP_MAX(p) MP_G(GETSTAT(p, WIZ))

#define SPELL_G(v) G(v)
#define SPELL_DMG(p, sp) SPELL_G(GETSTAT(p, INT)) + HS(sp)
#define SPELL_COST(dmg, y, no_bdmg) (no_bdmg ? 0 : dmg) + dmg / (1 << y)

#define BUF_DURATION(ra) 20 * (RARE_MAX - ra) / RARE_MAX
#define BUF_DMG(sp_dmg, duration) ((long) 2 * sp_dmg) / duration
#define BUF_TYPE_MASK 0xf
#define BUF_TYPE(sp) (sp->flags & BUF_TYPE_MASK)

#define MESGPROP_LID	"_/lid"
#define SETLID(x,y)	set_property_value(x, MESGPROP_LID, y + 1)
#define GETLID(x)	(get_property_value(x, MESGPROP_LID) - 1)

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

#define LIVING_SIZE (DB_INITIAL_SIZE/2)

#define HUNGER_Y	4
#define THIRST_Y	2
#define HUNGER_INC	(1 << (DAYTICK_Y - HUNGER_Y))
#define THIRST_INC	(1 << (DAYTICK_Y - THIRST_Y))

struct wts phys_wts[] = {
	{ "punch", "punches" },
	{ "peck", "pecks at" },
	{ "slash", "slashes" },
	{ "bite", "bites" },
	{ "strike", "strikes" },
};

const char *_spellc[] = {
	"",
	ANSI_FG_RED,
	ANSI_FG_BLUE,
	ANSI_FG_WHITE,
	ANSI_FG_YELLOW,
	ANSI_FG_MAGENTA,
	ANSI_BOLD ANSI_FG_BLACK,
};

struct wts buf_wts[] = {
	// HP
	{ "heal", "heals" },

	// - HP
	[16] =
	{ "bleed", "bleeds" },
	{ "burn", "burns" },

	// 32 MOV

	// - MOV
	[48] =
	{ "stunned", "stunned" },
	{ "", "" },
	{ "frozen", "frozen" },

	// MDMG
	[64] =
	{ "focus on attacking", "focuses on attacking" },
	{ "focus on fire", "focuses on fire" },

	// - MDMG
	[80] =
	{ "weaken", "weakens" },

	// MDEF
	[96] =
	{ "focus on defending", "focuses on defending" },
	{ "are protected by flames", "is protected by flames" },

	// - MDEF
	[112] =
	{ "are distracted", "is distracted" },

	// 128 DODGE

	// 144 - DDGE
};

static struct _spell spells[] = {
	{{"Heal", "", "" }, ELM_PHYSICAL, 3, 1, 2, AF_HP },

	{{"Focus", "", "" }, ELM_PHYSICAL, 15, 3, 1, AF_MDMG | AF_BUF, },
	{{"Fire Focus", "", "" }, ELM_FIRE, 15, 3, 1, AF_MDMG | AF_BUF, },

	{{"Cut", "", "" }, ELM_PHYSICAL, 15, 1, 2, AF_NEG, },
	{{"Fireball", "", "" }, ELM_FIRE, 3, 1, 2, AF_NEG, }, // 1/4 chance of burning

	{{"Weaken", "", "" }, ELM_PHYSICAL, 15, 3, 1, AF_MDMG | AF_BUF | AF_NEG, },
	{{"Distract", "", "" }, ELM_PHYSICAL, 15, 3, 1, AF_MDEF | AF_BUF | AF_NEG, },

	{{"Freeze", "", "" }, ELM_ICE, 10, 2, 4, AF_MOV | AF_NEG, },

	{{"Lava Shield", "", "" }, ELM_FIRE, 15, 3, 1, AF_MDEF | AF_BUF, },
	/* {{"Wind Veil", "", "" }, AF_COMBAT_DODGE, }, */
	/* {{"Stone Skin", "", "" }, AF_COMBAT_DEFENSE, }, */
};

enum elm_type weakness[] = {
	ELM_VAMP, // physical
	ELM_ICE, // fire
	ELM_FIRE, // ice
	ELM_DARK, // air
	ELM_AIR, // earth
	ELM_VAMP, // spirit
	ELM_SPIRIT, // vamp
	ELM_EARTH, // dark
};

enum living_flags {
	LF_NONE,
	LF_SITTING,
};

static struct living living[LIVING_SIZE],
		     *living_cur = &living[0];

static inline short
randd_dmg(short dmg)
{
	register unsigned short xx = 1 + (random() & 7);
	return xx = dmg + ((dmg * xx * xx * xx) >> 9);
}

static inline short
dmg_get(enum elm_type dmg_type, short dmg,
	short def, enum elm_type def_type)
{
	if (dmg > 0) {
		if (dmg_type == weakness[def_type])
			dmg *= 2;
		else if (weakness[dmg_type] == def_type)
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
dodge_get(struct living *att)
{
	int d = RAND_MAX / 4;
	short ad = EV(att, DODGE),
	      dd = EV(att->target, DODGE);

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

static inline int
dodge(struct living *att, struct wts wts)
{
	struct living *tar = att->target;
	if (!EV(tar, MOV) && dodge_get(att)) {
		notify_wts_to(tar->who, att->who, "dodge", "dodges", "'s %s", wts.a);
		return 1;
	} else
		return 0;
}

static inline void
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

struct living *
living_get(dbref who)
{
	int lid = GETLID(who);
	return lid < 0 ? NULL : &living[lid];
}

static inline char const *
sp_color(struct _spell *_sp)
{
	return BUF_TYPE(_sp) == AF_HP
		&& !(_sp->flags & AF_NEG)
		? ANSI_BOLD ANSI_FG_GREEN
		: _spellc[_sp->elm_type];
}

static inline struct wts *
debuf_wts(struct _spell *_sp)
{
	register unsigned char mask = _sp->flags;
	register unsigned idx = (BUF_TYPE(_sp) << 1) + ((mask >> 4) & 1);
	idx = (idx << 4) + _sp->elm_type;
	return &buf_wts[idx];
}

static void
debuf_notify(dbref who, struct debuf *d, short val)
{
	char buf[BUFSIZ];
	register struct _spell *_sp = d->_sp;
	char const *color = sp_color(_sp);
	struct wts *wts = debuf_wts(_sp);

	if (val)
		snprintf(buf, sizeof(buf), " (%s%d%s)", color, val, ANSI_RESET);
	else
		*buf = '\0';

	notify_wts(who, wts->a, wts->b, "%s", buf);
}

static inline struct effective *
eff_get(struct living *liv, unsigned char bt)
{
	return &liv->e[bt];
}

static inline int
debuf_start(dbref who, struct spell *sp, short val)
{
	struct _spell *_sp = sp->_sp;
	struct living *liv;
	struct debuf *d;
	int i;

	liv = living_get(who);
	bassert(liv);
	if (liv->debuf_mask) {
		i = __builtin_ffs(~liv->debuf_mask);
		if (!i)
			return -1;
		i--;
	} else
		i = 0;

	d = &liv->debufs[i];
	d->_sp = _sp;
	d->duration = BUF_DURATION(_sp->ra);
	d->val = BUF_DMG(val, d->duration);

	i = 1 << i;
	liv->debuf_mask |= i;

	struct effective *e = eff_get(liv, BUF_TYPE(_sp));
	e->mask |= i;
	e->value += d->val;

	debuf_notify(who, d, 0);

	return 0;
}

static inline void
spell_init(struct spell *sp, dbref player, unsigned intelligence, unsigned idx)
{
	struct _spell *_sp = &spells[idx];
	sp->_sp = _sp;
	sp->val = SPELL_DMG(player, _sp);
	sp->cost = SPELL_COST(sp->val, _sp->y, _sp->flags & AF_BUF);
}

static inline void
spells_init(struct spell sps[8], dbref player)
{
	char const *str = GETCURSPELLS(player);
	unsigned i = 0;
	unsigned intelligence = GETSTAT(player, INT);
	memset(sps, 0, sizeof(struct spell) * 8);

	if (str && *str != '\0')
		for (; i < 8; i ++) {
			char *end;
			spell_init(&sps[i], player, intelligence,
			      strtoul(str, &end, 0));
			str = end;
			if (*str == '\0') {
				i++;
				break;
			}
			str++;
		}

	for (; i < 8; i++)
		spell_init(&sps[i], player, intelligence, 0);
}

static inline enum elm_type
elm_type_next(struct living *liv, register unsigned char a)
{
	return a ? liv->debufs[__builtin_ffs(a) - 1]._sp->elm_type
		: ELM_PHYSICAL;
}

static inline enum elm_type
mdef_type(struct living *liv)
{
	return elm_type_next(liv, EM(liv, MDEF));
}

static inline unsigned char
mdmg_type(struct living *liv)
{
	return elm_type_next(liv, EM(liv, MDMG));
}

static inline void
debuf_end(struct living *liv, unsigned i)
{
	struct debuf *d = &liv->debufs[i];
	struct effective *e = eff_get(liv, BUF_TYPE(d->_sp));
	i = 1 << i;

	liv->debuf_mask ^= i;
	e->mask ^= i;
	e->value -= d->val;
}

static inline void
debufs_end(struct living *liv)
{
	register unsigned mask, i, aux;

	for (mask = liv->debuf_mask, i = 0;
	     (aux = __builtin_ffs(mask));
	     i++, mask >>= aux)

		 debuf_end(liv, i += aux - 1);
}

static inline void
living_dead(struct living *liv)
{
	bassert(liv);
	liv->target = NULL;
	liv->hp = HP_MAX(liv->who);
	liv->mp = MP_MAX(liv->who);
	liv->thirst = liv->hunger = 0;
	debufs_end(liv);
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

static void
kill(dbref attacker, dbref target)
{
	struct living *att = attacker > 0 ? living_get(attacker) : NULL;
	struct living *tar = living_get(target);
	bassert(tar);

	notify_wts(target, "die", "dies", "");

	if (attacker > 0 && Typeof(attacker) == TYPE_PLAYER)
		xp_award(attacker, target);

	if (Typeof(target) == TYPE_PLAYER) {
		dbref loc = getloc(target);
		moveto(target, PLAYER_HOME(target));
		untmp_clean(tar->descr, target, loc);
		SETKLOCK(target, 0);
		do_map(tar->descr, target);
	} else {
		if (tar->target) {
			dbref tartar = tar->target->who;
			SETKLOCK(tartar, GETKLOCK(tartar) - GETAGGRO(target));
		}
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

static int
hp_add(dbref attacker, dbref target, short amt)
{
	struct living *tar = living_get(target);
	bassert(tar);
	short hp = tar->hp;
	int ret = 0;
	hp += amt;
	if (hp <= 0) {
		hp = 1;
		ret = 1;
		kill(attacker, target);
	} else {
		register unsigned short max = HP_MAX(target);
		if (hp > max)
			hp = max;
	}
	tar->hp = hp;
	return ret;
}

static int
spell_cast(struct living *att, struct living *tar, unsigned slot)
{
	struct spell sp = att->spells[slot];
	struct _spell *_sp = sp._sp;

	unsigned mana = att->mp;
	char a[BUFSIZ]; // FIXME way too big?
	char b[BUFSIZ];
	char c[BUFSIZ];
	struct wts wts = { a, b };
	dbref attacker = att->who,
	      target = att->target->who;

	char const *color = sp_color(_sp);

	if (mana < sp.cost)
		return -1;

	snprintf(a, sizeof(a), "%s%s"ANSI_RESET, color, _sp->o.name);
	memcpy(b, a, sizeof(a));

	mana -= sp.cost;
	att->mp = mana > 0 ? mana : 0;

	short val = dmg_get(_sp->elm_type, sp.val, EV(tar, MDEF), mdef_type(tar));

	if (_sp->flags & AF_NEG) {
		val = -val;
		if (dodge(att, wts))
			return 0;
	} else
		target = attacker;

	snprintf(b, sizeof(b), "casts %s on", a);
	snprintf(c, sizeof(c), "cast %s on", a);
	wts.a = c;

	notify_attack(attacker, target, wts, 0, color, val);

	if (random() < (RAND_MAX >> _sp->y))
		debuf_start(target, &sp, val);

	return hp_add(attacker, target, val);
}

static inline int
spells_cast(struct living *att, struct living *tar)
{
	register unsigned i, d, combo = att->combo;
	unsigned enough_mp = 1;

	for (i = 0; enough_mp && (d = __builtin_ffs(combo)); combo >>= d) {
		switch (spell_cast(att, tar, (i += d) - 1)) {
		case -1: enough_mp = 0;
			 break;
		case 1:  return 1;
		}
	}

	if (!enough_mp)
		notify(att->who, "Not enough mana.");

	return 0;
}

static inline void
attack(struct living *p, int player_attack)
{
	struct living *att, *tar;
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

	mask = EM(att, MOV);

	if (mask) {
		register unsigned i = __builtin_ffs(mask) - 1;
		debuf_notify(attacker, &att->debufs[i], 0);
	} else if (!spells_cast(att, tar) && !dodge(att, att->wts)) {
		enum elm_type at = mdmg_type(att);
		enum elm_type dt = mdef_type(tar);
		dbref target = tar->who;
		short aval = -dmg_get(ELM_PHYSICAL, EV(att, DMG), EV(tar, DEF) + EV(tar, MDEF), dt);
		short bval = -dmg_get(at, EV(att, MDMG), EV(tar, MDEF), dt);
		char const *color = _spellc[at];
		notify_attack(attacker, target, att->wts, aval, color, bval);
		hp_add(attacker, target, aval + bval);
	}
}

static inline void
target_try(struct living *me, dbref target)
{
	struct living *tar;

	if (me->target)
		return;

	tar = living_get(target);
	bassert(tar);

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
	struct living *att = living_get(player),
		      *tar;

	bassert(att);

	if (FLAGS(here) & HAVEN
	    || target == NOTHING
	    || player == target
#if RESTRICT_KILL
	    || ( Typeof(target) == TYPE_PLAYER
		 && !((FLAGS(target) & KILL_OK)
		      && (FLAGS(player) & KILL_OK)))
#endif
	    || !(tar = living_get(target)))
	{
		notify(player, "You can't target that.");
		return;
	}

	// this happens when we clone living ones
	if (tar->who != target)
		tar = living_put(target);

	do_stand_silent(player);
	att->target = tar;
	att->descr = descr;
	notify(player, "You form a combat pose.");
}

static inline void
do_killing_update(struct living *liv)
{
	struct living *livt = liv->target;

	if (!livt || livt == liv)
		return;

	// target is no longer here
	if (getloc(livt->who) != getloc(liv->who)) {
		liv->target = NULL;
		return;
	}

	target_try(liv->target, liv->who);
	attack(liv, 1);
}

static void
respawn(dbref who)
{
	if (who < 0 || Typeof(who) != TYPE_THING)
		return;
	dbref where = THING_HOME(who);
	moveto(who, where);
	notify_except_fmt(DBFETCH(where)->contents, who,
			  "%s appears.", NAME(who));
}

void
do_status(dbref player)
{
	// TODO optimize EV / EM
	struct living *liv = living_get(player);
	notify_fmt(player, "hp %u/%u\tmp %u/%u\tstuck 0x%x\n"
		   "dodge %d\tcombo 0x%x \tdebuf_mask 0x%x\n"
		   "damage %d\tmdamage %d\tmdmg_mask 0x%x\n"
		   "defense %d\tmdefense %d\tmdef_mask 0x%x\n"
		   "hunger %u\tthirst %u",
		   liv->hp, HP_MAX(player), liv->mp, MP_MAX(player), EM(liv, MOV),
		   EV(liv, DODGE), liv->combo, liv->debuf_mask,
		   EV(liv, DMG), EV(liv, MDMG), EM(liv, MDMG),
		   EV(liv, DEF), EV(liv, MDEF), EM(liv, MDEF),
		   liv->hunger, liv->thirst);
}

void
do_heal(int descr, dbref player, const char *name)
{
	dbref here = getloc(player);
	dbref target;
	struct living *tar;

	if (strcmp(name, "me")) {
		target = contents_find(descr, player, here, name);

	} else
		target = player;

	if (!(FLAGS(player) & WIZARD)
	    || target < 0
	    || !(tar = living_get(target)))
		notify(player, "You can't do that.");
	return;

	tar->hp = HP_MAX(target);
	tar->mp = MP_MAX(target);
	tar->hunger = tar->thirst = 0;
	debufs_end(tar);
	notify_wts_to(player, target, "heal", "heals", "");
}

static inline void
living_init(struct living *liv, dbref who)
{
	int i;
	living_cur = liv;
	memset(liv, 0, sizeof(struct living));
	liv->who = who;
	liv->mob = mob_get(who);
	liv->wts = phys_wts[liv->mob ? liv->mob->wt : GETWTS(who)];
	liv->hunger = liv->thirst = 0;
	liv->flags = GETSAT(who) ? LF_SITTING : 0;
	liv->combo = GETCOMBO(who);
	liv->hp = HP_MAX(who);
	liv->mp = MP_MAX(who);

	EV(liv, DMG) = DMG_BASE(who);
	EV(liv, DODGE) = DODGE_BASE(who);

	spells_init(liv->spells, who);

	SETLID(who, living_cur - living);

	for (i = 0; i < EQ_MAX; i++) {
		register dbref eq = GETEQ(who, i);
		if (eq > 0)
			equip_calc(liv, eq);
	}
}

struct living *
living_put(dbref who)
{
	register const unsigned m = LIVING_SIZE;
	register struct living *liv;
	register unsigned i;

	if (who < 0 || Typeof(who) == TYPE_GARBAGE)
		return NULL;

	for (liv = living_cur, i = 0;
	     i < m;
	     i++, liv++)
	{
		if (liv > living + m)
			liv = living;

		if (liv->who <= 0) {
			living_init(liv, who);
			return liv;
		}
	}

	return NULL;
}

static inline void
do_respawn_init()
{
	dbref tmp = DBFETCH((dbref) 0)->contents, next;

	while (tmp > 0) {
		next = DBFETCH(tmp)->next;
		if (living_get(tmp))
			respawn(tmp);

		tmp = next;
	}
}

void
do_living_init()
{
	register PropPtr p = get_property((dbref) 0, "_/living");

	if (p) {
		const char *liv_s = PropDataStr(p), *l;
		for (l = liv_s; l; l = strchr(l, '#'))
			living_put((dbref) atoi(++l));
	}

	do_respawn_init();
}

static inline void
debufs_process(struct living *liv)
{
	register unsigned mask, i, aux;
	short hpi = 0, dmg;
	struct debuf *d, *hd;

	for (mask = liv->debuf_mask, i = 0;
	     (aux = __builtin_ffs(mask));
	     i++, mask >>= aux)
	{
		i += aux - 1;
		d = &liv->debufs[i];
		d->duration--;
		if (d->duration == 0)
			debuf_end(liv, i);
		// wtf is this special code?
		else if (BUF_TYPE(d->_sp) == AF_HP) {
			dmg = dmg_get(d->_sp->elm_type, d->val,
				      EV(liv, MDEF), mdef_type(liv));
			hd = d;

			hpi += dmg;
		}
	}

	if (hpi) {
		debuf_notify(liv->who, hd, hpi);
		hp_add(NOTHING, liv->who, hpi);
	}
}

static inline void
huth_notify(dbref who, unsigned v, unsigned char y, char const *m[4])
{
	static unsigned const n[] = {
		1 << (DAY_Y - 1),
		1 << (DAY_Y - 2),
		1 << (DAY_Y - 3)
	};

	register unsigned aux;

	if (v == n[2])
		notify(who, m[0]);
	else if (v == (aux = n[1]))
		notify(who, m[1]);
	else if (v == (aux += n[0]))
		notify(who, m[2]);
	else if (v == (aux += n[2]))
		notify(who, m[3]);
	else if (v > aux) {
		short val = -(HP_MAX(who) >> 3);
		hp_add(NOTHING, who, val);
	}
}

static inline void
living_update(struct living *n)
{
	static char const *thirst_msg[] = {
		"You are thirsty.",
		"You are very thirsty.",
		"you are dehydrated.",
		"You are dying of thirst."
	};

	static char const *hunger_msg[] = {
		"You are hungry.",
		"You are very hungry.",
		"You are starving.",
		"You are starving to death."
	};

	dbref who = n->who;

	if (n->respawn_in > 0) {
		if (!--n->respawn_in)
			respawn(who);

		return;
	} else {
		int y = 9 - 2 * (n->flags | LF_SITTING);

		int max = HP_MAX(who);
		int cur = n->hp + (max >> y);
		n->hp = cur > max ? max : cur;

		max = MP_MAX(who);
		cur = n->mp + (max >> y);
		n->mp = cur > max ? max : cur;

		huth_notify(n->who, n->thirst += THIRST_INC,
			    THIRST_Y, thirst_msg);

		huth_notify(n->who, n->hunger += HUNGER_INC,
			    HUNGER_Y, hunger_msg);
	}

	debufs_process(n);
	do_killing_update(n);
}

void
livings_update()
{
	register struct living *n, *e;

	for (n = &living[0], e = n + LIVING_SIZE;
	     n < e;
	     n++)

		if (n->who > 0)
			living_update(n);
		else if (n < living_cur)
			living_cur = n;
}

void
do_advitam(int descr, dbref player, const char *name)
{
	dbref here = getloc(player);
	dbref target = contents_find(descr, player, here, name);

	if (!(FLAGS(player) & WIZARD)
	    || target == NOTHING
	    || living_get(target)
	    || OWNER(target) != player) {
		notify(player, "You can't do that.");
		return;
	}

	living_put(target);
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
do_living_save()
{
	static const unsigned m = LIVING_SIZE;
	register struct living *n = &living[0];
	char buf[BUFFER_LEN], *b = buf;
	size_t len = 0;
	unsigned i;
	PData mydat;

	// TODO? use mask?
	for (i = 0; i < m; i++, n++)
		if (n->who > 0 && (Typeof(n->who) == TYPE_PLAYER
				   || Typeof(n->who) == TYPE_THING))
		{
			register size_t d;
			d = snprintf(b, sizeof(buf) - len, "#%d ", n->who);
			len += d;
			if (len > sizeof(buf))
				abort();
			b += d;
		}

	mydat.flags = PROP_STRTYP;
	mydat.data.str = buf;
	set_property((dbref) 0, "_/living", &mydat);
}

void
do_train(dbref player, const char *attrib, const char *amount_s)
{
	struct living *liv = living_get(player);
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
		EV(liv, DMG) += DMG_G(c + amount) - DMG_G(c);
		break;
	case DEX:
		EV(liv, DODGE) += DODGE_G(c + amount) - DODGE_G(c);
		break;
	}

	SETSPEND(player, avail - amount);
	notify_fmt(player, "Your %s increases %d time(s).", attrib, amount);
}

int
kill_v(int *drmap, int descr, dbref player, char const *opcs)
{
	char *end;
	if (isdigit(*opcs)) {
		unsigned combo = strtol(opcs, &end, 0);
		SETCOMBO(player, combo);
		living_get(player)->combo = combo;
		notify_fmt(player, "Set combo to 0x%x.", combo);
		return end - opcs;
	} else if (*opcs == 'c' && isdigit(opcs[1])) {
		struct living *p = living_get(player);
		unsigned slot = strtol(opcs + 1, &end, 0);
		bassert(p);

		if (!p->target)
			p->target = p;

		spell_cast(p, p->target, slot);
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
	living_get(player)->flags |= LF_SITTING;

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
	living_get(player)->flags ^= LF_SITTING;
	notify_wts(player, "stand", "stands", " up");
	return 0;
}

void
do_stand(dbref player)
{
	if (do_stand_silent(player))
		notify(player, "You are already standing.");
}
