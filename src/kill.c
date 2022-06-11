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

static inline OBJ *
body(OBJ *player, OBJ *mob)
{
	char buf[32];
	struct object_skeleton o = { "", "", "" };
	snprintf(buf, sizeof(buf), "%s's body.", mob->name);
	o.name = buf;
	OBJ *dead_mob = object_add(o, mob->location);
	OBJ *tmp;
	unsigned n = 0;

	for (; (tmp = mob->contents); ) {
		CBUG(tmp->type != TYPE_GARBAGE);
		/* if (object_get(tmp)->type == TYPE_GARBAGE) */
		/* 	continue; */
		if (tmp->type == TYPE_EQUIPMENT) {
			EQU *etmp = &tmp->sp.equipment;
			unequip(mob, EQL(etmp->eqw));
		}
		object_move(tmp, dead_mob);
		n++;
	}

	if (n > 0) {
		onotifyf(mob, "%s's body drops to the ground.", mob->name);
		return dead_mob;
	} else {
		recycle(player, dead_mob);
		return NULL;
	}
}

static inline void
_xp_award(OBJ *player, unsigned const xp)
{
	ENT *eplayer = &player->sp.entity;
	unsigned cxp = eplayer->cxp;
	notifyf(player, "You gain %u xp!", xp);
	cxp += xp;
	while (cxp >= 1000) {
		notify(player, "You level up!");
		cxp -= 1000;
		eplayer->lvl += 1;
		eplayer->spend += 2;
	}
	eplayer->cxp = cxp;
}

static inline void
xp_award(OBJ *player, OBJ *target)
{
	ENT *eplayer = &player->sp.entity,
	    *etarget = &target->sp.entity;
	unsigned x = eplayer->lvl;
	unsigned y = etarget->lvl;
	_xp_award(player, xp_get(x, y));
}

OBJ *
kill_target(OBJ *player, OBJ *target)
{
	ENT *eplayer = player ? &player->sp.entity : NULL;
	ENT *etarget = &target->sp.entity;

	notify_wts(target, "die", "dies", "");

	body(player, target);

	if (player) {
		CBUG(player->type != TYPE_ENTITY)
		xp_award(player, target);
		eplayer->target = NULL;
	}

	CBUG(target->type != TYPE_ENTITY);

	if (etarget->target && (etarget->flags & EF_AGGRO)) {
		OBJ *tartar = etarget->target;
		ENT *etartar = &tartar->sp.entity;
		etartar->klock --;
	}

	OBJ *loc = target->location;
	ROO *rloc = &loc->sp.room;

	if ((rloc->flags & RF_TEMP) && !(etarget->flags & EF_PLAYER)) {
		recycle(player, target);
		geo_clean(target, loc);
		return NULL;
	}

	etarget->klock = 0;
	etarget->target = NULL;
	etarget->hp = 1;
	etarget->mp = 1;
	etarget->thirst = etarget->hunger = 0;

	debufs_end(target);
	object_move(target, object_get((dbref) 0));
	geo_clean(target, loc);
	look_around(target);
	web_bars(target);

	return target;
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
		cspell_heal(player, target, aval + bval);
	}
}

void
do_kill(command_t *cmd)
{
	const char *what = cmd->argv[1];
	OBJ *player = object_get(cmd->player);
	OBJ *here = player->location;
	ROO *rhere = &here->sp.room;
	OBJ *target = strcmp(what, "me")
		? ematch_near(player, what)
		: player;

	if (object_ref(here) == 0 || (rhere->flags & RF_HAVEN)) {
		notify(player, "You may not kill here");
		return;
	}

	CBUG(player->type != TYPE_ENTITY);

	if (!target
	    || player == target
	    || target->type != TYPE_ENTITY)
	{
		notify(player, "You can't target that.");
		return;
	}

	player->sp.entity.target = target;
	/* notify(player, "You form a combat pose."); */
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

	do_stand_silent(player);
	attack(player);
}

void
do_status(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	// TODO optimize MOB_EV / MOB_EM
	notifyf(player, "hp %u/%u\tmp %u/%u\tstuck 0x%x\n"
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
                notify(player, "You can't do that.");
                return;
        }

	etarget = &target->sp.entity;
	etarget->hp = HP_MAX(etarget);
	etarget->mp = MP_MAX(etarget);
	etarget->hunger = etarget->thirst = 0;
	debufs_end(target);
	notify_wts_to(player, target, "heal", "heals", "");
	web_bars(target);
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
		notify(player, "You can't do that.");
		return;
	}

	birth(target);
	notifyf(player, "You infuse %s with life.", target->name);
}

void
do_givexp(command_t *cmd, const char *name, const char *amount)
{
	OBJ *player = object_get(cmd->player),
	    *target = ematch_near(player, name);
	ENT *eplayer = &player->sp.entity;
	int amt = strtol(amount, NULL, 0);

	if (!(eplayer->flags & EF_WIZARD) || !target)
		notify(player, "You can't do that.");
	else
		_xp_award(target, amt);
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
		  notify(player, "Invalid attribute.");
		  return;
	}

	int avail = eplayer->spend;
	int amount = *amount_s ? atoi(amount_s) : 1;

	if (amount > avail) {
		  notify(player, "Not enough points.");
		  return;
	}

	unsigned c = ATTR(eplayer, attr);
	ATTR(eplayer, attr) += amount;

	switch (attr) {
	case ATTR_STR:
		EFFECT(eplayer, DMG).value += DMG_G(c + amount) - DMG_G(c);
		break;
	case ATTR_DEX:
		EFFECT(eplayer, DODGE).value += DODGE_G(c + amount) - DODGE_G(c);
		break;
	}

	eplayer->spend = avail - amount;
	notifyf(player, "Your %s increases %d time(s).", attrib, amount);
        web_stats(player);
}

int
kill_v(OBJ *player, char const *opcs)
{
	ENT *eplayer = &player->sp.entity;
	char *end;
	if (isdigit(*opcs)) {
		unsigned combo = strtol(opcs, &end, 0);
		eplayer->combo = combo;
		notifyf(player, "Set combo to 0x%x.", combo);
		return end - opcs;
	} else if (*opcs == 'c' && isdigit(opcs[1])) {
		unsigned slot = strtol(opcs + 1, &end, 0);
                OBJ *target = eplayer->target;
		if (player->location == 0) {
			notify(player, "You may not cast spells in room 0");
		} else {
			spell_cast(player, target, slot);
		}
		return end - opcs;
	} else
		return 0;
}

void
sit(OBJ *player, const char *name)
{
	ENT *eplayer = &player->sp.entity;

	if (eplayer->flags & EF_SITTING) {
		notify(player, "You are already sitting.");
		return;
	}

	if (!*name) {
		notify_wts(player, "sit", "sits", " on the ground");
		eplayer->flags |= EF_SITTING;
		eplayer->sat = NULL;

		/* warn("%d sits on the ground\n", player); */
		return;
	}

	OBJ *seat = ematch_near(player, name);
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
	eplayer->flags |= EF_SITTING;
	eplayer->sat = seat;

	notify_wts(player, "sit", "sits", " on %s", seat->name);
}

void
do_sit(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	const char *name = cmd->argv[1];
        sit(player, name);
}

int
do_stand_silent(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;

	if (!(eplayer->flags & EF_SITTING))
		return 1;

	OBJ *chair = eplayer->sat;

	if (chair) {
		SETSEATN(chair, GETSEATN(chair) - 1);
		eplayer->sat = NULL;
	}

	eplayer->flags ^= EF_SITTING;
	notify_wts(player, "stand", "stands", " up");
	return 0;
}

void
stand(OBJ *player) {
	if (do_stand_silent(player))
		notify(player, "You are already standing.");
}

void
do_stand(command_t *cmd)
{
        stand(object_get(cmd->player));
}
