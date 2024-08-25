#include "entity.h"

#include "io.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <qhash.h>

#include "spacetime.h"
#include "mcp.h"
#include "map.h"
#include "defaults.h"
#include "params.h"
#include "match.h"
#include "view.h"
#include "room.h"
#include "equipment.h"
#include "spell.h"
#include "debug.h"
#include "mob.h"
#include "utils.h"
#include "uapi/wts.h"

#define HUNGER_Y	4
#define THIRST_Y	2

unsigned long long huth_inc[2] = {
	(1 << (DAYTICK_Y - THIRST_Y)),
	(1 << (DAYTICK_Y - HUNGER_Y)),
};

long tmp_hds;

static const char *rarity_str[] = {
	ANSI_BOLD ANSI_FG_BLACK "Poor" ANSI_RESET,
	"",
	ANSI_BOLD "Uncommon" ANSI_RESET,
	ANSI_BOLD ANSI_FG_CYAN "Rare" ANSI_RESET,
	ANSI_BOLD ANSI_FG_GREEN "Epic" ANSI_RESET,
	ANSI_BOLD ANSI_FG_MAGENTA "Mythical" ANSI_RESET
};

ENT ent_get(dbref ref) {
	ENT ent;
	hash_cget(tmp_hds, &ent, &ref, sizeof(ref));
	return ent;
}

void ent_set(dbref ref, ENT *tmp) {
	hash_cput(tmp_hds, &ref, sizeof(ref), tmp, sizeof(ENT));
}

void ent_tmp_reset(ENT *ent) {
	ent->last_observed = NOTHING;
	ent->select = 0;
	ent->huth_n[0] = ent->huth_n[1] = 0;
}

void birth(ENT *eplayer)
{
	eplayer->huth[HUTH_THIRST] = eplayer->huth[HUTH_HUNGER] = eplayer->select =
		eplayer->huth_n[HUTH_THIRST] = eplayer->huth_n[HUTH_HUNGER] = 0;
	eplayer->combo = 0;
	eplayer->hp = HP_MAX(eplayer);
	eplayer->mp = MP_MAX(eplayer);
	eplayer->target = NOTHING;
	eplayer->sat = NOTHING;

	EFFECT(eplayer, DMG).value = DMG_BASE(eplayer);
	EFFECT(eplayer, DODGE).value = DODGE_BASE(eplayer);

	int i;

	for (i = 0; i < ES_MAX; i++) {
		register dbref eq = EQUIP(eplayer, i);

		if (eq <= 0)
			continue;

		OBJ oeq = obj_get(eq);
		EQU *eeq = &oeq.sp.equipment;
		CBUG(equip_affect(eplayer, eeq));
	}
}

void
recycle(dbref thing_ref)
{
	dbref rest_ref, first_ref;
	OBJ thing = obj_get(thing_ref);

	OBJ owner = obj_get(thing.owner);
	unsigned owner_wizard = ent_get(thing.owner).flags & EF_WIZARD;

	switch (thing.type) {
	case TYPE_ROOM:
		{
			ROO *rthing = &thing.sp.room;
			if (!owner_wizard)
				owner.value += ROOM_COST;
			if (rthing->flags & RF_TEMP) {
				struct hash_cursor c = contents_iter(thing_ref);
				dbref first_ref;
				while ((first_ref = contents_next(&c)))
					if (obj_get(first_ref).type != TYPE_ENTITY ||
						!(ent_get(first_ref).flags & EF_PLAYER))
						recycle(first_ref);
			}
			map_delete(thing_ref);
		}

		break;
	case TYPE_PLANT:
	case TYPE_SEAT:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
	case TYPE_MINERAL:
		if (!owner_wizard)
			owner.value += thing.value;

		OBJ thingloc = obj_get(thing.location);

		if (thingloc.type != TYPE_ROOM)
			break;

		if (thingloc.sp.room.flags & RF_TEMP) {
			struct hash_cursor c = contents_iter(thing_ref);
			while ((first_ref = contents_next(&c)))
				recycle(first_ref);
		}

		break;
	}

	FOR_ALL(rest_ref) {
		OBJ rest = obj_get(rest_ref);
		switch (rest.type) {
		case TYPE_ROOM:
			if (rest.owner == thing_ref)
				rest.owner = GOD;
			break;
		case TYPE_PLANT:
		case TYPE_SEAT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
		case TYPE_MINERAL:
			if (rest.owner == thing_ref)
				rest.owner = GOD;
			break;
		case TYPE_ENTITY:
			{
				ENT erest = ent_get(rest_ref);
				if (erest.home == thing_ref) {
					erest.home = PLAYER_START;
					ent_set(rest_ref, &erest);
				}
			}

			break;
		}
		obj_set(rest_ref, &rest);
	}
	obj_set(thing.owner, &owner);

	struct hash_cursor c = contents_iter(thing_ref);
	while ((first_ref = contents_next(&c))) {
		OBJ first = obj_get(first_ref);
		if (first.type == TYPE_ENTITY) {
			ENT efirst = ent_get(first_ref);
			if (efirst.flags & EF_PLAYER) {
				enter(first_ref, efirst.home, E_NULL);
				continue;
			}
		}

		recycle(first_ref);
	}

	object_move(thing_ref, NOTHING);

	object_free(thing_ref);
	object_clear(&thing);
	thing.name = (char*) strdup("<garbage>");
	thing.description = (char *) strdup("<recyclable>");
	thing.type = TYPE_GARBAGE;
	obj_set(thing_ref, &thing);
}

static inline int
entities_aggro(dbref player_ref)
{
	ENT eplayer = ent_get(player_ref);
	dbref tmp_ref;
	int klock = 0;

	struct hash_cursor c = contents_iter(obj_get(player_ref).location);
	while ((tmp_ref = contents_next(&c))) {
		if (obj_get(tmp_ref).type != TYPE_ENTITY)
			continue;

		ENT etmp = ent_get(tmp_ref);

		if (etmp.flags & EF_AGGRO) {
			etmp.target = player_ref;
			klock++;
		}

		ent_set(tmp_ref, &etmp);
	}

	eplayer.klock += klock;
	ent_set(player_ref, &eplayer);
	return klock;
}

void
enter(dbref player_ref, dbref loc_ref, enum exit e)
{
	OBJ player = obj_get(player_ref);
	dbref old_loc_ref = player.location;

	st_run(player_ref, "leave");
	if (e == E_NULL)
		nd_owritef(player_ref, "%s teleports out.\n", player.name);
	object_move(player_ref, loc_ref);
	room_clean(old_loc_ref);
	if (e == E_NULL)
		nd_owritef(player_ref, "%s teleports in.\n", player.name);
	st_run(player_ref, "enter");
	entities_aggro(player_ref);
}

int
payfor(dbref who_ref, OBJ *who, int cost)
{
	if (ent_get(who_ref).flags & EF_WIZARD)
		return 1;

	if (who->value >= cost) {
		who->value -= cost;
		return 1;
	} else {
		return 0;
	}
}

int
controls(dbref who_ref, dbref what_ref)
{
	if (what_ref == NOTHING)
		return 0;

	OBJ what = obj_get(what_ref);
	if (what.type == TYPE_GARBAGE)
		return 0;

	/* Zombies and puppets use the permissions of their owner */
	OBJ who = obj_get(who_ref);
	if (who.type != TYPE_ENTITY)
		who_ref = who.owner;

	/* Wizard controls everything */
	if (ent_get(who_ref).flags & EF_WIZARD) {
		if(what.owner == GOD && who_ref == GOD)
			/* Only God controls God's objects */
			return 0;
		else
			return 1;
	}

	/* owners control their own stuff */
	return (who_ref == what.owner);
}

#define BUFF(...) buf_l += snprintf(&buf[buf_l], BUFSIZ - buf_l, __VA_ARGS__)

const char *
unparse(dbref loc_ref)
{
	static char buf[BUFSIZ];
        size_t buf_l = 0;

	if (loc_ref == NOTHING)
		return "*NOTHING*";

	OBJ loc = obj_get(loc_ref);

	if (loc.type == TYPE_EQUIPMENT) {
		EQU *eloc = &loc.sp.equipment;

		if (eloc->eqw) {
			unsigned n = eloc->rare;

			if (n != 1)
				BUFF("(%s) ", rarity_str[n]);
		}
	}

	BUFF("%s(#%d)", loc.name, loc_ref);

	buf[buf_l] = '\0';
	return buf;
}

void
sit(dbref player_ref, ENT *eplayer, char *name)
{
	if (eplayer->flags & EF_SITTING) {
		nd_writef(player_ref, "You are already sitting.\n");
		return;
	}

	if (!*name) {
		notify_wts(player_ref, "sit", "sits", " on the ground");
		eplayer->flags |= EF_SITTING;
		eplayer->sat = NOTHING;
		return;
	}

	dbref seat_ref = ematch_near(player_ref, name);
	if (seat_ref == NOTHING) {
		nd_writef(player_ref, "Invalid target.\n");
		return;
	}

	OBJ seat = obj_get(seat_ref);

	if (seat.type != TYPE_SEAT) {
		nd_writef(player_ref, "You can't sit on that.\n");
		return;
	}

	SEA *sseat = &seat.sp.seat;

	if (sseat->quantity >= sseat->capacity) {
		nd_writef(player_ref, "No seats available.\n");
		return;
	}

	sseat->quantity += 1;
	eplayer->flags |= EF_SITTING;
	eplayer->sat = seat_ref;
	notify_wts(player_ref, "sit", "sits", " on %s", seat.name);
}

int
stand_silent(dbref player_ref, ENT *eplayer)
{
	if (!(eplayer->flags & EF_SITTING))
		return 1;

	if (eplayer->sat != NOTHING) {
		OBJ chair = obj_get(eplayer->sat);
		SEA *schair = &chair.sp.seat;
		schair->quantity--;
		obj_set(eplayer->sat, &chair);
		eplayer->sat = NOTHING;
	}

	eplayer->flags ^= EF_SITTING;
	notify_wts(player_ref, "stand", "stands", " up");
	return 0;
}

void
stand(dbref player_ref, ENT *eplayer) {
	if (stand_silent(player_ref, eplayer))
		nd_writef(player_ref, "You are already standing.\n");
}

void
look_around(dbref player_ref)
{
	mcp_look(player_ref, obj_get(player_ref).location);
}

int
equip_affect(ENT *ewho, EQU *equ)
{
	register int msv = equ->msv,
		 eqw = equ->eqw,
		 eql = EQL(eqw),
		 eqt = EQT(eqw);

	unsigned aux = 0;

	switch (eql) {
	case ES_RHAND:
		if (ewho->attr[ATTR_STR] < msv)
			return 1;
		EFFECT(ewho, DMG).value += DMG_WEAPON(equ);
		/* ewho->wts = WTS_WEAPON(equ); */
		break;

	case ES_HEAD:
	case ES_PANTS:
		aux = 1;
	case ES_CHEST:

		switch (eqt) {
		case ARMOR_LIGHT:
			if (ewho->attr[ATTR_DEX] < msv)
				return 1;
			aux += 2;
			break;
		case ARMOR_MEDIUM:
			msv /= 2;
			if (ewho->attr[ATTR_STR] < msv
			    || ewho->attr[ATTR_DEX] < msv)
				return 1;
			aux += 1;
			break;
		case ARMOR_HEAVY:
			if (ewho->attr[ATTR_STR] < msv)
				return 1;
		}
		aux = DEF_ARMOR(equ, aux);
		EFFECT(ewho, DEF).value += aux;
		int pd = EFFECT(ewho, DODGE).value - DODGE_ARMOR(aux);
		EFFECT(ewho, DODGE).value = pd > 0 ? pd : 0;
	}

	return 0;
}

int
equip(dbref who_ref, dbref eq_ref)
{
	OBJ eq = obj_get(eq_ref);
	CBUG(obj_get(who_ref).type != TYPE_ENTITY);
	ENT ewho = ent_get(who_ref);
	CBUG(eq.type != TYPE_EQUIPMENT);
	EQU *eeq = &eq.sp.equipment;
	unsigned eql = EQL(eeq->eqw);

	if (!eql || EQUIP(&ewho, eql) > 0
	    || equip_affect(&ewho, eeq))
		return 1;

	EQUIP(&ewho, eql) = eq_ref;
	eeq->flags |= EQF_EQUIPPED;

	nd_writef(who_ref, "You equip %s.\n", eq.name);
	ent_set(who_ref, &ewho);
	mcp_stats(who_ref);
	mcp_content_out(who_ref, eq_ref);
	mcp_equipment(who_ref);
	return 0;
}

dbref
unequip(dbref player_ref, unsigned eql)
{
	ENT eplayer = ent_get(player_ref);
	dbref eq_ref = EQUIP(&eplayer, eql);
	unsigned eqt, aux;

	if (eq_ref == NOTHING)
		return NOTHING;

	OBJ eq = obj_get(eq_ref);
	EQU *eeq = &eq.sp.equipment;
	eqt = EQT(eeq->eqw);
	aux = 0;

	switch (eql) {
	case ES_RHAND:
		EFFECT(&eplayer, DMG).value -= DMG_WEAPON(eeq);
		break;
	case ES_PANTS:
	case ES_HEAD:
		aux = 1;
	case ES_CHEST:
		switch (eqt) {
		case ARMOR_LIGHT: aux += 2; break;
		case ARMOR_MEDIUM: aux += 1; break;
		}
		aux = DEF_ARMOR(eeq, aux);
		EFFECT(&eplayer, DEF).value -= aux;
		EFFECT(&eplayer, DODGE).value += DODGE_ARMOR(aux);
	}

	EQUIP(&eplayer, eql) = NOTHING;
	eeq->flags &= ~EQF_EQUIPPED;
	ent_set(player_ref, &eplayer);
	mcp_content_in(player_ref, eq_ref);
	mcp_stats(player_ref);
	mcp_equipment(player_ref);
	return eq_ref;
}

static inline unsigned
entity_xp(ENT *eplayer, dbref target_ref)
{
	ENT etarget = ent_get(target_ref);

	// alternatively (2000/x)*y/x
	if (!eplayer->lvl)
		return 0;

	unsigned r = 254 * etarget.lvl / (eplayer->lvl * eplayer->lvl);
	if (r < 0)
		return 0;
	else
		return r;
}

static inline void
_entity_award(dbref player_ref, ENT *eplayer, unsigned const xp)
{
	unsigned cxp = eplayer->cxp;
	nd_writef(player_ref, "You gain %u xp!\n", xp);
	cxp += xp;
	while (cxp >= 1000) {
		nd_writef(player_ref, "You level up!\n");
		cxp -= 1000;
		eplayer->lvl += 1;
		eplayer->spend += 2;
	}
	eplayer->cxp = cxp;
}

static inline void
entity_award(dbref player_ref, ENT *eplayer, dbref target_ref)
{
	_entity_award(player_ref, eplayer, entity_xp(eplayer, target_ref));
}

static inline dbref
entity_body(dbref mob_ref)
{
	dbref tmp_ref;
	char buf[32];
	struct object_skeleton skel = { .name = "", .description = "" };
	OBJ mob = obj_get(mob_ref), dead_mob;
	snprintf(buf, sizeof(buf), "%s's body.", mob.name);
	skel.name = buf;
	dbref dead_mob_ref = object_add(&dead_mob, &skel, mob.location, NULL);
	unsigned n = 0;

	struct hash_cursor c = contents_iter(mob_ref);
	while ((tmp_ref = contents_next(&c))) {
		OBJ tmp = obj_get(tmp_ref);
		if (tmp.type == TYPE_GARBAGE)
			continue;
		if (tmp.type == TYPE_EQUIPMENT) {
			EQU *etmp = &tmp.sp.equipment;
			unequip(mob_ref, EQL(etmp->eqw));
		}
		object_move(tmp_ref, dead_mob_ref);
		n++;
	}

	if (n > 0) {
		nd_owritef(mob_ref, "%s's body drops to the ground.\n", mob.name);
		obj_set(dead_mob_ref, &dead_mob);
		return dead_mob_ref;
	} else {
		recycle(dead_mob_ref);
		return NOTHING;
	}
}

static inline dbref
entity_kill(dbref player_ref, dbref target_ref)
{
	notify_wts(target_ref, "die", "dies", "");

	entity_body(target_ref);

	if (player_ref != NOTHING) {
		CBUG(obj_get(player_ref).type != TYPE_ENTITY)
		ENT eplayer = ent_get(player_ref);
		entity_award(player_ref, &eplayer, target_ref);
		eplayer.target = NOTHING;
		ent_set(player_ref, &eplayer);
	}

	OBJ target = obj_get(target_ref);
	CBUG(target.type != TYPE_ENTITY);
	ENT etarget = ent_get(target_ref);

	if (etarget.target && (etarget.flags & EF_AGGRO)) {
		ENT etartar = ent_get(etarget.target);
		etartar.klock --;
		ent_set(etarget.target, &etartar);
	}

	dbref loc_ref = target.location;

	if ((obj_get(target.location).sp.room.flags & RF_TEMP) && !(etarget.flags & EF_PLAYER)) {
		recycle(target_ref);
		room_clean(loc_ref);
		return NOTHING;
	}

	etarget.klock = 0;
	etarget.target = NOTHING;
	etarget.hp = 1;
	etarget.mp = 1;
	ent_tmp_reset(&etarget);
	etarget.huth[HUTH_THIRST] = etarget.huth[HUTH_HUNGER] = 0;
	debufs_end(&etarget);
	ent_set(target_ref, &etarget);
	st_start(target_ref);
	/* object_move(target, object_get((dbref) 0)); */
	room_clean(loc_ref);
	look_around(target_ref);
	mcp_bars(target_ref);
	view(target_ref);

	return target_ref;
}

int
entity_damage(dbref player_ref, dbref target_ref, short amt)
{
	ENT etarget = ent_get(target_ref);
	int hp = etarget.hp;
	hp += amt;

	if (!amt)
		return 0;

	if (hp <= 0) {
		entity_kill(player_ref, target_ref);
		return 1;
	}

	register unsigned short max = HP_MAX(&etarget);
	if (hp > max)
		hp = max;

	etarget.hp = hp;
	ent_set(target_ref, &etarget);

	return 0;
}

void
do_look_at(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd), thing_ref;
	char *name = argv[1];

	if (*name == '\0') {
		thing_ref = obj_get(player_ref).location;
	} else if (
			(thing_ref = ematch_absolute(name)) == NOTHING
			&& (thing_ref = ematch_here(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_me(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_near(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_mine(player_ref, name)) == NOTHING
		  )
	{
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	switch (obj_get(thing_ref).type) {
	case TYPE_ROOM:
		view(player_ref);
	default:
		mcp_look(player_ref, thing_ref);
		break;
	}
}

static void
respawn(dbref player_ref)
{
	ENT eplayer = ent_get(player_ref);
	OBJ player = obj_get(player_ref);

	nd_owritef(player_ref, "%s disappears.\n", player.name);

	if (eplayer.flags & EF_PLAYER) {
		struct cmd_dir cd;
		cd.rep = STARTING_POSITION;
		cd.dir = '\0';
		st_teleport(player_ref, cd);
	} else {
		/* warn("respawning %d to %d\n", who, where); */
		object_move(player_ref, eplayer.home);
	}

	nd_owritef(player_ref, "%s appears.\n", player.name);
}

static inline int
huth_notify(dbref player_ref, ENT *eplayer, enum huth type)
{
	static char const *msg[] = {
		"You are thirsty.\n",
		"You are very thirsty.\n",
		"you are dehydrated.\n",
		"You are dying of thirst.\n"
		"You are hungry.\n",
		"You are very hungry.\n",
		"You are starving.\n",
		"You are starving to death.\n"
	};

	static unsigned const n[] = {
		1 << (DAY_Y - 1),
		1 << (DAY_Y - 2),
		1 << (DAY_Y - 3)
	};

	unsigned long long v;
	unsigned char rn = eplayer->huth_n[type];
	v = eplayer->huth[type] += huth_inc[type];

	char const **m = msg + 4 * type;

	if (rn >= 4) {
		short val = -(HP_MAX(eplayer) >> 3);
		return entity_damage(NOTHING, player_ref, val);
	} else if (rn >= 3) {
		if (v >= 2 * n[1] + n[2]) {
			nd_writef(player_ref, m[3]);
			rn += 1;
		}
	} else if (rn >= 2) {
		if (v >= 2 * n[1]) {
			nd_writef(player_ref, m[2]);
			rn += 1;
		}
	} else if (rn >= 1) {
		if (v >= n[1]) {
			nd_writef(player_ref, m[1]);
			rn += 1;
		}
	} else if (v >= n[2]) {
		nd_writef(player_ref, m[0]);
		rn += 1;
	}

	eplayer->huth_n[type] = rn;
	return 0;
}

static inline unsigned char
d20()
{
	return (random() % 20) + 1;
}

static unsigned char
entity_ac(ENT *eplayer)
{
	return 10 + MODIFIER(eplayer, ATTR_DEX);
}

// returns 1 if target dodges
static inline int
dodge_get(ENT *eplayer)
{
	return d20() < entity_ac(eplayer);
}

int
kill_dodge(dbref player_ref, char *wts)
{
	ENT eplayer = ent_get(player_ref);
	ENT etarget = ent_get(eplayer.target);
	if (!EFFECT(&etarget, MOV).value && dodge_get(&eplayer)) {
		notify_wts_to(eplayer.target, player_ref, "dodge", "dodges", "'s %s", wts);
		return 1;
	} else
		return 0;
}

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

static inline void
attack(dbref player_ref, ENT *eplayer)
{
	register unsigned char mask;

	mask = EFFECT(eplayer, MOV).mask;

	if (mask) {
		register unsigned i = __builtin_ffs(mask) - 1;
		debuf_notify(player_ref, &eplayer->debufs[i], 0);
		return;
	}

	if (eplayer->target == NOTHING)
		return;

	ENT etarget = ent_get(eplayer->target);
	char *wts = wts_map[eplayer->wtso];

	dbref eq_ref = EQUIP(eplayer, ES_RHAND);

	if (eq_ref != NOTHING)
		wts = wts_map[EQT(obj_get(eq_ref).sp.equipment.eqw)];

	if (spells_cast(player_ref, eplayer, eplayer->target) && !kill_dodge(player_ref, wts)) {
		enum element at = ELEMENT_NEXT(eplayer, MDMG);
		enum element dt = ELEMENT_NEXT(&etarget, MDEF);
		short aval = -kill_dmg(ELM_PHYSICAL, EFFECT(eplayer, DMG).value, EFFECT(&etarget, DEF).value + EFFECT(&etarget, MDEF).value, dt);
		short bval = -kill_dmg(at, EFFECT(eplayer, MDMG).value, EFFECT(&etarget, MDEF).value, dt);
		char const *color = ELEMENT(at)->color;
		notify_attack(player_ref, eplayer->target, wts, aval, color, bval);
		entity_damage(player_ref, eplayer->target, aval + bval);
	}
}

static inline void
kill_update(dbref player_ref, ENT *eplayer, double dt)
{
	if (eplayer->target == NOTHING)
		return;

	OBJ target = obj_get(eplayer->target);
	OBJ player = obj_get(player_ref);

	if (target.type == TYPE_GARBAGE || target.location != player.location) {
		eplayer->target = NOTHING;
		return;
	}

	dbref target_ref = eplayer->target;
	ENT etarget = ent_get(target_ref);

	if (etarget.target == NOTHING) {
		etarget.target = player_ref;
		ent_set(target_ref, &etarget);
	}

	stand_silent(player_ref, eplayer);

	unsigned short ohp = etarget.hp;
	unsigned short omp = eplayer->mp;
	attack(player_ref, eplayer);
	if (eplayer->mp != omp)
		mcp_bars(player_ref);
	if (etarget.hp != ohp)
		mcp_bars(target_ref);
}

void
entity_update(dbref player_ref, double dt)
{
	OBJ player = obj_get(player_ref);
	ENT eplayer = ent_get(player_ref);
	unsigned short ohp = eplayer.hp;
	unsigned short omp = eplayer.mp;

	if (!(eplayer.flags & EF_PLAYER)) {
		/* warn("%d hp %d/%d\n", player, eplayer.hp, HP_MAX(player)); */
		if (eplayer.hp == HP_MAX(&eplayer)) {
			/* warn("%d's hp is maximum\n", player); */

			if ((eplayer.flags & EF_SITTING)) {
				/* warn("%d is sitting so they shall stand\n", player); */
				stand(player_ref, &eplayer);
			}

			if (player.location == 0) {
				/* warn("%d is located at 0, so they shall respawn\n", player); */
				ent_set(player_ref, &eplayer);
				respawn(player_ref);
				player = obj_get(player_ref);
				eplayer = ent_get(player_ref);
			}
		} else {
			/* warn("%d's hp is not maximum\n", player); */
			if (eplayer.target == NOTHING && !(eplayer.flags & EF_SITTING)) {
				/* warn("%d is not sitting so they shall sit\n", player); */
				sit(player_ref, &eplayer, "");
			}
		}
	}

	if (eplayer.flags & EF_SITTING) {
		int div = 100;
		int max, cur;
		entity_damage(NOTHING, player_ref, dt * HP_MAX(&eplayer) / div);
		eplayer = ent_get(player_ref);
		max = MP_MAX(&eplayer);
		cur = eplayer.mp + (max / div);
		eplayer.mp = cur > max ? max : cur;
	}

        CBUG(player.type == TYPE_GARBAGE);

	if (player.location == 0)
		return;

        /* if mob dies, return */
	if (!(huth_notify(player_ref, &eplayer, HUTH_THIRST)
		|| huth_notify(player_ref, &eplayer, HUTH_HUNGER)
                || debufs_process(player_ref, &eplayer)))

		kill_update(player_ref, &eplayer, dt);

	if (player.type == TYPE_GARBAGE)
		return;

	ent_set(player_ref, &eplayer);
	if (eplayer.hp != ohp || eplayer.mp != omp)
		mcp_bars(player_ref);
}

void
stats_init(ENT *enu, SENT *sk)
{
	unsigned char stat = sk->stat;
	int lvl = sk->lvl, spend, i, sp,
	    v = sk->lvl_v ? sk->lvl_v : 0xf;

	lvl += random() & v;

	if (!stat)
		stat = 0x1f;

	spend = 1 + lvl;
	for (i = 0; i < ATTR_MAX; i++)
		if (stat & (1<<i)) {
			sp = random() % spend;
			enu->attr[i] = sp;
		}

	enu->lvl = lvl;
}

static inline int
bird_is(SENT *sk)
{
	return sk->wt == WT_PECK;
}

static inline dbref
entity_add(enum mob_type mid, dbref where_ref, enum biome biome, long long pdn) {
	struct object_skeleton *obj_skel = ENTITY_SKELETON(mid);
	CBUG(obj_skel->type != S_TYPE_ENTITY);
	struct entity_skeleton *mob_skel = &obj_skel->sp.entity;

	if ((bird_is(mob_skel) && !pdn)
	    || (!NIGHT_IS && (mob_skel->type == ELM_DARK || mob_skel->type == ELM_VAMP))
	    || random() >= (RAND_MAX >> mob_skel->y))
		return NOTHING;

	if (!((1 << biome) & mob_skel->biomes))
		return NOTHING;

	OBJ obj;
	dbref obj_ref = object_add(&obj, obj_skel, where_ref, NULL);
	obj_set(obj_ref, &obj);
	return obj_ref;
}

void
entities_add(dbref where_ref, enum biome biome, long long pdn) {
	unsigned mid;

	for (mid = 1; mid < MOB_MAX; mid++)
		entity_add(mid, where_ref, biome, pdn);
}

void
reroll(dbref player_ref, ENT *eplayer) {
	int i;

	for (i = 0; i < ATTR_MAX; i++)
		eplayer->attr[i] = d20();

	EFFECT(eplayer, DMG).value = DMG_BASE(eplayer);
	EFFECT(eplayer, DODGE).value = DODGE_BASE(eplayer);
	mcp_stats(player_ref);
	mcp_bars(player_ref);
}

void
do_reroll(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd),
	      thing_ref = player_ref;

	char *what = argv[1];

	if (
			argc > 1 && (thing_ref = ematch_me(player_ref, what)) == NOTHING
			&& (thing_ref = ematch_near(player_ref, what)) == NOTHING
			&& (thing_ref = ematch_mine(player_ref, what)) == NOTHING
	   ) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	ENT eplayer = ent_get(player_ref);
	reroll(player_ref, &eplayer);
	ent_set(player_ref, &eplayer);
}

void entities_init() {
	tmp_hds = hash_init();
}
