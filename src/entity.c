#include "entity.h"
#include "io.h"
#include "debug.h"
#include "mcp.h"
#include "map.h"
#include "defaults.h"
#include "params.h"
#include <string.h>
#include "match.h"
#include "props.h"
#include "view.h"
#include "room.h"

#define MESGPROP_SEATM	"_seat_m"
#define GETSEATM(x)	get_property_value(x, MESGPROP_SEATM)

#define MESGPROP_SEATN	"_seat_n"
#define GETSEATN(x)	get_property_value(x, MESGPROP_SEATN)
#define SETSEATN(x, y)	set_property_value(x, MESGPROP_SEATN, y)

static const char *rarity_str[] = {
	ANSI_BOLD ANSI_FG_BLACK "Poor" ANSI_RESET,
	"",
	ANSI_BOLD "Uncommon" ANSI_RESET,
	ANSI_BOLD ANSI_FG_CYAN "Rare" ANSI_RESET,
	ANSI_BOLD ANSI_FG_GREEN "Epic" ANSI_RESET,
	ANSI_BOLD ANSI_FG_MAGENTA "Mythical" ANSI_RESET
};

void
recycle(OBJ *player, OBJ *thing)
{
	extern OBJ *recyclable;
	ENT *eplayer = &player->sp.entity;
	static int depth = 0;
	OBJ *first, *rest;
	int looplimit;

	OBJ *owner = thing->owner;
	ENT *eowner = &owner->sp.entity;

	depth++;
	switch (thing->type) {
	case TYPE_ROOM:
		{
			ROO *rthing = &thing->sp.room;
			if (!(eowner->flags & EF_WIZARD))
				owner->value += ROOM_COST;
			if (rthing->flags & RF_TEMP)
				for (first = thing->contents; first; first = rest) {
					rest = first->next;

					if (first->type == TYPE_ENTITY) {
						ENT *efirst = &first->sp.entity;
						if (efirst->flags & EF_PLAYER)
							continue;
					}

					recycle(player, first);
				}
			anotifyf(thing, "You feel a wrenching sensation...");
			map_delete(thing);
		}

		break;
	case TYPE_PLANT:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (!(eowner->flags & EF_WIZARD))
			owner->value += thing->value;

		OBJ *thingloc = thing->location;

		if (thingloc->type != TYPE_ROOM)
			break;

		ROO *rthingloc = &thingloc->sp.room;

		if (rthingloc->flags & RF_TEMP) {
			for (first = thing->contents; first; first = rest) {
				rest = first->next;
				recycle(player, first);
			}
		}

		break;
	}

	FOR_ALL(rest) {
		switch (rest->type) {
		case TYPE_ROOM:
			{
				ROO *rrest = &rest->sp.room;
				if (rrest->dropto == thing)
					rrest->dropto = NULL;
				if (rest->owner == thing)
					rest->owner = object_get(GOD);
			}

			break;
		case TYPE_PLANT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
			if (rest->owner == thing)
				rest->owner = object_get(GOD);
			break;
		case TYPE_ENTITY:
			{
				ENT *erest = &rest->sp.entity;
				if (erest->home == thing)
					erest->home = object_get(PLAYER_START);
			}

			break;
		}
		if (rest->contents == thing)
			rest->contents = thing->next;
		if (rest->next == thing)
			rest->next = thing->next;
	}

	looplimit = db_top;
	while ((looplimit-- > 0) && (first = thing->contents)) {
		if (first->type == TYPE_ENTITY) {
			ENT *efirst = &first->sp.entity;
			if (efirst->flags & EF_PLAYER) {
				enter(first, efirst->home);
				if (first->location == thing) {
					notifyf(eplayer, "Escaping teleport loop!  Going home.");
					object_move(first, object_get(PLAYER_START));
				}
				continue;
			}
		}

		recycle(player, first);
	}


	object_move(thing, NULL);

	depth--;

	object_free(thing);
	object_clear(thing);
	thing->name = (char*) strdup("<garbage>");
	thing->description = (char *) strdup("<recyclable>");
	thing->type = TYPE_GARBAGE;
	thing->next = recyclable;
	recyclable = thing;
}

void
enter(OBJ *player, OBJ *loc)
{
	OBJ *old = player->location;

	onotifyf(player, "%s has left.", player->name);
	object_move(player, loc);
	room_clean(player, old);
	onotifyf(player, "%s has arrived.", player->name);
	mobs_aggro(player);
	look_around(player);
}

int
controls(OBJ *who, OBJ *what)
{
	if (!what)
		return 0;

	if (what->type == TYPE_GARBAGE)
		return 0;

	/* Zombies and puppets use the permissions of their owner */
	if (who->type != TYPE_ENTITY)
		who = who->owner;

	ENT *ewho = &who->sp.entity;

	/* Wizard controls everything */
	if (ewho->flags & EF_WIZARD) {
#ifdef GOD_PRIV
		if(God(what->owner) && !God(who))
			/* Only God controls God's objects */
			return 0;
		else
#endif
		return 1;
	}

	/* owners control their own stuff */
	return (who == what->owner);
}

static inline int
controls_link(OBJ *who, OBJ *what)
{
	switch (what->type) {
	case TYPE_ROOM:
		if (controls(who, what->sp.room.dropto))
			return 1;
		return 0;

	case TYPE_ENTITY:
		if (controls(who, what->sp.entity.home))
			return 1;
		return 0;

	default:
		return 0;
	}
}

#define BUFF(...) buf_l += snprintf(&buf[buf_l], BUFFER_LEN - buf_l, __VA_ARGS__)

const char *
unparse(OBJ *player, OBJ *loc)
{
	static char buf[BUFFER_LEN];
        size_t buf_l = 0;

	if (player && player->type != TYPE_ENTITY)
		player = player->owner;

	if (!loc)
		return "*NOTHING*";

	if (loc->type == TYPE_EQUIPMENT) {
		EQU *eloc = &loc->sp.equipment;

		if (eloc->eqw) {
			unsigned n = eloc->rare;

			if (n != 1)
				BUFF("(%s) ", rarity_str[n]);
		}
	}

	BUFF("%s", loc->name);

	if (!player || controls_link(player, loc))
		BUFF("(#%d)", object_ref(loc));

	buf[buf_l] = '\0';
	return buf;
}

void
sit(OBJ *player, const char *name)
{
	ENT *eplayer = &player->sp.entity;

	if (eplayer->flags & EF_SITTING) {
		notify(eplayer, "You are already sitting.");
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
		notify(eplayer, "You can't sit on that.");
		return;
	}

	int cur = GETSEATN(seat);
	if (cur >= max) {
		notify(eplayer, "No seats available.");
		return;
	}

	SETSEATN(seat, cur + 1);
	eplayer->flags |= EF_SITTING;
	eplayer->sat = seat;

	notify_wts(player, "sit", "sits", " on %s", seat->name);
}

int
stand_silent(OBJ *player)
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

int
cando(OBJ *player, OBJ *thing, const char *default_fail_msg)
{
	ENT *eplayer = &player->sp.entity;

	if (!thing) {
		notify(eplayer, default_fail_msg);
		return 0;
	}

	CBUG(player->type != TYPE_ENTITY);

	if (eplayer->klock) {
		notify(eplayer, "You can not do that right now.");
		return 0;
	}

	stand_silent(player);

	return 1;
}

static void
look_contents(OBJ *player, OBJ *loc, const char *contents_name)
{
	ENT *eplayer = &player->sp.entity;
        char buf[BUFSIZ];
        size_t buf_l = 0;
	OBJ *thing;

	/* check to see if there is anything there */
	if (loc->contents) {
                FOR_LIST(thing, loc->contents) {
			if (thing == player)
				continue;
			buf_l += snprintf(&buf[buf_l], BUFSIZ - buf_l,
					"\n%s", unparse(player, thing));
                }
	}

        buf[buf_l] = '\0';

        notifyf(eplayer, "%s%s", contents_name, buf);
}

void
look_room(OBJ *player, OBJ *loc)
{
	ENT *eplayer = &player->sp.entity;
	char const *description = "";
	CBUG(loc->type != TYPE_ROOM);
	cando(player, loc, 0);

	if (mcp_look(player, loc)) {
		notify(eplayer, unparse(player, loc));
		notify(eplayer, description);
		look_contents(player, loc, "You see:");
	}
}

void
look_around(OBJ *player)
{
	look_room(player, player->location);
}

static inline unsigned
entity_xp(ENT *eplayer, ENT *etarget)
{
	// alternatively (2000/x)*y/x
	unsigned r = 254 * etarget->lvl / (eplayer->lvl * eplayer->lvl);
	if (r < 0)
		return 0;
	else
		return r;
}

static inline void
_entity_award(ENT *eplayer, unsigned const xp)
{
	unsigned cxp = eplayer->cxp;
	notifyf(eplayer, "You gain %u xp!", xp);
	cxp += xp;
	while (cxp >= 1000) {
		notify(eplayer, "You level up!");
		cxp -= 1000;
		eplayer->lvl += 1;
		eplayer->spend += 2;
	}
	eplayer->cxp = cxp;
}

static inline void
entity_award(ENT *eplayer, ENT *etarget)
{
	_entity_award(eplayer, entity_xp(eplayer, etarget));
}

static inline OBJ *
entity_body(OBJ *player, OBJ *mob)
{
	char buf[32];
	struct object_skeleton o = { "", "", "" };
	snprintf(buf, sizeof(buf), "%s's body.", mob->name);
	o.name = buf;
	OBJ *dead_mob = object_add(&o, mob->location);
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

static inline OBJ *
entity_kill(OBJ *player, OBJ *target)
{
	ENT *eplayer = player ? &player->sp.entity : NULL;
	ENT *etarget = &target->sp.entity;

	notify_wts(target, "die", "dies", "");

	entity_body(player, target);

	if (player) {
		CBUG(player->type != TYPE_ENTITY)
		entity_award(eplayer, etarget);
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
		room_clean(target, loc);
		return NULL;
	}

	etarget->klock = 0;
	etarget->target = NULL;
	etarget->hp = 1;
	etarget->mp = 1;
	etarget->thirst = etarget->hunger = 0;

	debufs_end(etarget);
	object_move(target, object_get((dbref) 0));
	room_clean(target, loc);
	look_around(target);
	mcp_bars(etarget);

	return target;
}

int
entity_damage(OBJ *player, OBJ *target, short amt)
{
	ENT *etarget = &target->sp.entity;
	short hp = etarget->hp;
	int ret = 0;
	hp += amt;

	if (!amt)
		return ret;

	if (hp <= 0) {
		hp = 1;
		ret = 1;
		target = entity_kill(player, target);
	} else {
		register unsigned short max = HP_MAX(etarget);
		if (hp > max)
			hp = max;
	}

	if (target) {
		etarget->hp = hp;
		CBUG(target->type != TYPE_ENTITY);
		mcp_bars(etarget);
	}

	if (player && (player->type != TYPE_GARBAGE)) {
		CBUG(player->type != TYPE_ENTITY);
		ENT *eplayer = &player->sp.entity;
		mcp_bars(eplayer);
	}

	return ret;
}

void
art(int fd, const char *art)
{
	FILE *f;
	char buf[BUFFER_LEN];
        size_t len = strlen(art);

        if (!strcmp(art + len - 3, "txt")) {
                snprintf(buf, sizeof(buf), "../art/%s", art);

                if ((f = fopen(buf, "rb")) == NULL) 
                        return;

                while (fgets(buf, sizeof(buf) - 3, f))
                        descr_inband(fd, buf);

                fclose(f);
                descr_inband(fd, "\r\n");
        } else
                mcp_art(fd, art);
}

static void
look_simple(ENT *eplayer, OBJ *thing)
{
	char const *art_str = thing->art;

	if (art_str)
		art(eplayer->fd, art_str);

	if (thing->description) {
		notify(eplayer, thing->description);
	} else if (!art_str) {
		notify(eplayer, "You see nothing special.");
	}
}

void
do_look_at(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *thing;

	if (*name == '\0') {
		thing = player->location;
	} else if (
			!(thing = ematch_absolute(name))
			&& !(thing = ematch_here(player, name))
			&& !(thing = ematch_me(player, name))
			&& !(thing = ematch_near(player, name))
			&& !(thing = ematch_mine(player, name))
		  )
	{
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	switch (thing->type) {
	case TYPE_ROOM:
		look_room(player, thing);
		view(player);
		break;
	case TYPE_ENTITY:
		if (mcp_look(player, thing)) {
			look_simple(eplayer, thing);
			look_contents(player, thing, "Carrying:");
		}
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (mcp_look(player, thing))
			look_simple(eplayer, thing);
		break;
	default:
		if (mcp_look(player, thing))
			look_simple(eplayer, thing);
		break;
	}
}

