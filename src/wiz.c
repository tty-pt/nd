/* $Header$ */

#include "io.h"
#include "entity.h"
#include "config.h"

/* Wizard-only commands */

#include <stdio.h>

#include <sys/resource.h>

#include <stdlib.h>

#include "params.h"
#include "defaults.h"
#include "match.h"
#include "player.h"
#include "debug.h"
#include "command.h"

void
do_teleport(command_t *cmd) {
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *arg1 = cmd->argv[1];
	const char *arg2 = cmd->argv[2];
	OBJ *victim;
	OBJ *destination;
	const char *to;

	/* get victim, destination */
	if (*arg2 == '\0') {
		victim = player;
		to = arg1;
	} else if (!(victim = ematch_all(player, arg1))) {
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	} else
		to = arg2;

	if(!God(player) && God(victim->owner)) {
		notify(eplayer, "God has already set that where He wants it to be.");
		return;
	}

	destination = ematch_all(player, to);

	if (!destination) {
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	switch (victim->type) {
	case TYPE_ENTITY:
		{
			ENT *evictim = &victim->sp.entity;
			if (!controls(player, victim) ||
					!controls(player, destination) ||
					!controls(player, victim->location) ||
					(object_item(destination) && !controls(player, destination->location))) {
				notify(eplayer, "Permission denied. (must control victim, dest, victim's loc, and dest's loc)");
				break;
			}
			if (destination->type != TYPE_ROOM) {
				notify(eplayer, "Bad destination.");
				break;
			}
			if (object_plc(victim, destination)) {
				notify(eplayer, "Objects can't contain themselves.");
				break;
			}
			notify(evictim, "You feel a wrenching sensation...");
			enter(victim, destination, E_NULL);
		}
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (object_plc(victim, destination)) {
			notify(eplayer, "You can't make a container contain itself!");
			break;
		}
		if (destination->type != TYPE_ROOM
			&& destination->type != TYPE_ENTITY
			&& !object_item(destination)) {
			notify(eplayer, "Bad destination.");
			break;
		}
		if (!(controls(player, destination) &&
			  (controls(player, victim) || controls(player, victim->location)))) {
			notify(eplayer, "Permission denied. (must control dest and be able to link to it, or control dest's loc)");
			break;
		}
		/* check for non-sticky dropto */
		if (destination->type == TYPE_ROOM) {
			ROO *rdestination = &destination->sp.room;
			if (rdestination->dropto)
				destination = rdestination->dropto;
		}
		object_move(victim, destination);
		notify(eplayer, "Teleported.");
		break;
	case TYPE_GARBAGE:
		notify(eplayer, "That object is in a place where magic cannot reach it.");
		break;
	case TYPE_ROOM:
	default:
		notify(eplayer, "You can't teleport that.");
		break;
	}
}

void
do_boot(command_t *cmd) {
	OBJ *player = cmd->player;
	CBUG(player->type != TYPE_ENTITY);
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *victim;

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "Only a Wizard player can boot someone off.");
		return;
	}

	victim = player_get(name);

	if (!victim) {
		notify(eplayer, "That player does not exist.");
		return;
	}

	if (victim->type != TYPE_ENTITY) {
		notify(eplayer, "You can only boot entities!");
	}

	ENT *evictim = &victim->sp.entity;

	if (God(victim)) {
		notify(eplayer, "You can't boot God!");
	}

	else {
		notify(evictim, "You have been booted off the game.");
		if (entity_boot(evictim)) {
			warn("BOOTED: %s(%d) by %s(%d)", victim->name,
					   object_ref(victim), player->name, object_ref(player));
			if (player != victim) {
				notifyf(eplayer, "You booted %s off!", victim->name);
			}
		} else {
			notifyf(eplayer, "%s is not connected.", victim->name);
		}
	}
}

static inline OBJ *
reverse(OBJ *list)
{
	OBJ *newlist = NULL;
	OBJ *rest;

	while (list) {
		rest = list->next;
		PUSH(list, newlist);
		list = rest;
	}

	return newlist;
}

static inline void
send_contents(OBJ *loc, OBJ *dest)
{
	OBJ *first, *rest;

	first = loc->contents;
	loc->contents = NULL;

	/* blast locations of everything in list */
	FOR_LIST(rest, first)
		rest->location = NULL;

	while (first) {
		rest = first->next;
		if (!object_item(first)) {
			object_move(first, loc);
		} else {
			object_move(first, object_plc(first, dest) ? loc : dest);
		}
		first = rest;
	}

	loc->contents = reverse(loc->contents);
}

void
do_toad(command_t *cmd) {
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	const char *recip = cmd->argv[2];
	OBJ *victim;
	OBJ *recipient;
	dbref stuff;
	char buf[BUFFER_LEN];

	CBUG(player->type != TYPE_ENTITY);

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "Only a Wizard player can turn an entity into a toad.");
		return;
	}

	victim = player_get(name);

	if (!victim) {
		notify(eplayer, "That player does not exist.");
		return;
	}

	if (God(victim)) {
		notify(eplayer, "You cannot @toad God.");
		if(!God(player)) {
			warn("TOAD ATTEMPT: %s(#%d) tried to toad God.", player->name, object_ref(player));
		}
		return;
	}

	if (!*recip) {
		/* FIXME: Make me a tunable parameter! */
		recipient = object_get(GOD);
	} else {
		recipient = player_get(recip);
		if (!recipient || recipient == victim) {
			notify(eplayer, "That recipient does not exist.");
			return;
		}
	}

	if (victim->type != TYPE_ENTITY)
		notify(eplayer, "You can only turn entities into toads!");

	ENT *evictim = &victim->sp.entity;

	if (!(God(player)) && (evictim->flags & EF_WIZARD)) {
		notify(eplayer, "You can't turn a Wizard into a toad.");
	} else {
		/* we're ok */
		/* do it */
		send_contents(victim, evictim->home);
		for (stuff = 0; stuff < db_top; stuff++) {
			OBJ *ostuff = object_get(stuff);
			if (ostuff->owner == victim) {
				switch (ostuff->type) {
				case TYPE_ROOM:
				case TYPE_CONSUMABLE:
				case TYPE_EQUIPMENT:
				case TYPE_THING:
					ostuff->owner = recipient;
					break;
				}
			}
		}

		/* notify people */
		notify(evictim, "You have been turned into a toad.");
		notifyf(eplayer, "You turned %s into a toad!", victim->name);
		warn("TOADED: %s(%d) by %s(%d)", victim->name, object_ref(victim), player->name, object_ref(player));
		/* reset name */
		player_delete(victim);
		snprintf(buf, sizeof(buf), "A slimy toad named %s", victim->name);
		free((void *) victim->name);
		victim->name = strdup(buf);

		entity_boot(evictim);

		victim->type = TYPE_THING;
		victim->owner = player;	/* you get it */
		victim->value = 1; /* don't let him keep his immense wealth */
	}
}

void
do_usage(command_t *cmd) {
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	struct rusage usage;

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "Permission denied. (@usage is wizard-only)");
		return;
	}
	getrusage(RUSAGE_SELF, &usage);

	notifyf(eplayer, "Performed %d input servicings.", usage.ru_inblock);
	notifyf(eplayer, "Performed %d output servicings.", usage.ru_oublock);
	notifyf(eplayer, "Sent %d messages over a socket.", usage.ru_msgsnd);
	notifyf(eplayer, "Received %d messages over a socket.", usage.ru_msgrcv);
	notifyf(eplayer, "Received %d signals.", usage.ru_nsignals);
	notifyf(eplayer, "Page faults NOT requiring physical I/O: %d", usage.ru_minflt);
	notifyf(eplayer, "Page faults REQUIRING physical I/O: %d", usage.ru_majflt);
	notifyf(eplayer, "Swapped out of main memory %d times.", usage.ru_nswap);
	notifyf(eplayer, "Voluntarily context switched %d times.", usage.ru_nvcsw);
	notifyf(eplayer, "Involuntarily context switched %d times.", usage.ru_nivcsw);
	notifyf(eplayer, "User time used: %d sec.", usage.ru_utime.tv_sec);
	notifyf(eplayer, "System time used: %d sec.", usage.ru_stime.tv_sec);
}
